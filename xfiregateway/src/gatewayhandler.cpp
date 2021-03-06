/*
 *  xfiregateway - Jabber Gateway for XFire.
 *  Copyright (C) 2006 by
 *          Herbert Poul <herbert.poul@gmail.com> / http://goim.us
 *    http://xfirelib.sphene.net/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "gatewayhandler.h"
#include "simplelib.h"

#include <gloox/gloox.h>

#include <xfirelib/xdebug.h>

namespace xfiregateway {
  using namespace gloox;

  GatewayHandler::GatewayHandler(XFireGateway *gateway) {
    this->gateway = gateway;
    this->comp = gateway->getComponent();
  }


  bool GatewayHandler::handleIq(Stanza *stanza) {
    XDEBUG(( "I'm in handleIq(Stanza)\n" ));
    if(stanza->xmlns() == "jabber:iq:register") {
      if(stanza->subtype() == StanzaIqGet) {
	XDEBUG(( "ok .. we're having jabber:iq:register with get :)\n" ));
	Tag *reply = new Tag( "iq" );
	reply->addAttribute( "id", stanza->id() );
	reply->addAttribute( "type", "result" );
	reply->addAttribute( "to", stanza->from().full() );
	reply->addAttribute( "from", gateway->getFQDN() );
	Tag *query = new Tag( "query" );
	query->addAttribute( "xmlns", "jabber:iq:register" );
	query->addChild( new Tag("instructions", "Enter your xfire username and password (This is still ALPHA!!).") );
	query->addChild( new Tag("username") );
	query->addChild( new Tag("password") );
	
	reply->addChild(query);
	comp->send( reply );

	return true;
      } else if(stanza->subtype() == StanzaIqSet) {
	bool sendsubscribe = false;
	Tag *query = stanza->findChild( "query" );
	if(query->hasChild( "remove" )) {
	  XDEBUG(( "Someone wants to be removed...\n" ));
	  User *user = gateway->getUserByJID( stanza->from().bare() );
	  if(!user) {
	    return false;
	  }
	  gateway->removeUser( user );

	  Tag *reply = new Tag("iq");
	  reply->addAttribute( "type", "result" );
	  reply->addAttribute( "from", gateway->getFQDN() );
	  reply->addAttribute( "to", stanza->from().full() );
	  reply->addAttribute( "id", stanza->id() );
	  comp->send( reply );

	  reply = new Tag( "presence" );
	  reply->addAttribute( "type", "unsubscribe" );
	  reply->addAttribute( "from", gateway->getFQDN() );
	  reply->addAttribute( "to", stanza->from().full() );
	  comp->send( reply );
	  reply = new Tag("presence");
	  reply->addAttribute( "type", "unsubscribed" );
	  reply->addAttribute( "to", stanza->from().full() );
	  reply->addAttribute( "from", gateway->getFQDN() );
	  comp->send( reply );
	  
	  return true;
	}
	XDEBUG(( "Someone wants to register or change password\n" ));
	std::string jid = stanza->from().bare();
	Tag *usernametag = query->findChild("username");
	Tag *passwordtag = query->findChild("password");
	std::string username = usernametag->cdata();
	std::string password = passwordtag->cdata();
	XDEBUG(( "Username: %s Password: %s\n",
		 username.c_str(),password.c_str() ));

	User *user = gateway->getUserByJID(jid);
	if(user == NULL) {
	  XDEBUG(( "Registering...\n" ));
	  user = gateway->getUserByXFireName(username);
	  if(user) {
	    XDEBUG(( "User already exists .. :( \n" ));
	    Stanza *r = new Stanza(stanza);//->clone();
	    Tag *error = new Tag("error");
	    error->addAttribute( "code", "409" );
	    error->addAttribute( "type", "cancel" );
	    Tag *conflict = new Tag("conflict");
	    conflict->addAttribute( "xmlns", "urn:ietf:params:xml:ns:xmpp-stanzas" );
	    conflict->setCData( "Someone else already registered with this xfire username (" + user->jid + ")" );
	    error->addChild( conflict );
	    r->addChild( error );
	    comp->send( r );
	  } else {
	    XDEBUG(( "We got a new user, YEAH\n" ));
	    user = new User(gateway);
	    std::string lowerjid = SimpleLib::stringToLower(jid);
	    user->jid = lowerjid;
	    user->name = username;
	    user->password = password;
	    gateway->addUser( user );
	    sendsubscribe = true;
	    XDEBUG(( "Added user, sending response...\n" ));
	  }
	} else {
	  XDEBUG(( "Changing Password...\n" ));
	  user->password = password;
	  gateway->writeUserFile();
	}
        Tag *reply = new Tag( "iq" );
        reply->addAttribute( "id", stanza->id() );
        reply->addAttribute( "type", "result" );
        reply->addAttribute( "to", stanza->from().full() );
        reply->addAttribute( "from", gateway->getFQDN() );
        Tag *rquery = new Tag( "query" );
        rquery->addAttribute( "xmlns", "jabber:iq:register" );

        reply->addChild(rquery);
        comp->send( reply );
	if(sendsubscribe) {
	  reply = new Tag("presence");
	  reply->addAttribute( "from", gateway->getFQDN() );
	  reply->addAttribute( "to", stanza->from().bare() );
	  reply->addAttribute( "type", "subscribe" );

	  comp->send( reply );
	}
	return true;
      }
    } else if(stanza->xmlns() == "jabber:iq:gateway") {
      if(stanza->subtype() == StanzaIqGet || stanza->subtype() == StanzaIqSet) {
	Tag *reply = new Tag("iq");
	reply->addAttribute( "from", gateway->getFQDN() );
	reply->addAttribute( "to", stanza->from().full() );
	reply->addAttribute( "id", stanza->id() );
	reply->addAttribute( "type", "result" );
	
	Tag *replyquery = new Tag("query");
	replyquery->addAttribute( "xmlns", "jabber:iq:gateway" );

	Tag *query = stanza->findChild( "query" );
	Tag *prompt = query->findChild( "prompt" );
	if(!prompt) {
	  replyquery->addChild( new Tag( "desc", "Please enter the XFire username of the person you would like to contact." ) );
	  replyquery->addChild( new Tag( "prompt", "XFire Username" ) );
	} else {
	  replyquery->addChild( new Tag( "prompt", prompt->cdata() + "@" + gateway->getFQDN() ) );
	  replyquery->addChild( new Tag( "jid", prompt->cdata() + "@" + gateway->getFQDN() ) );
	}
	reply->addChild(replyquery);
	comp->send(reply);
	return true;
      }
    }
    return false;
  }
  bool GatewayHandler::handleIqID(Stanza *stanza, int context) {
    XDEBUG(( "I'm in handleIqID(Stanza, int = %d)\n",context ));
    return false;
  }


  void GatewayHandler::handlePresence(Stanza *stanza) {
    if(stanza->subtype() == StanzaPresenceError) {
      XERROR(( "Received error presence from %s - XML: %s\n", stanza->from().full().c_str(), stanza->xml().c_str() ));
      return;
    }
    XDEBUG(( "Received presence from %s\n", stanza->from().full().c_str() ));
    User *user = gateway->getUserByJID( stanza->from().bare() );
    if(!user) {
      XINFO(( "Received presence from someone not registered ?! (%s), bare(%s)\n", stanza->from().full().c_str(), stanza->from().bare().c_str() ));
    } else {
      if(stanza->subtype() == StanzaS10nSubscribe && stanza->to().username() == "") {
	Tag *reply = new Tag("presence");
	reply->addAttribute( "to", stanza->from().bare() );
	reply->addAttribute( "from", stanza->to().bare() );
	reply->addAttribute( "type", "subscribed" );
	comp->send( reply );
      }
      user->receivedPresence( stanza );
    }
  }
  void GatewayHandler::handleSubscription(Stanza *stanza) {
    handlePresence(stanza);
  }

  void GatewayHandler::handleMessage(Stanza *stanza, MessageSession *message) {
    XDEBUG(( "Received message from %s to %s \n",
	     stanza->from().full().c_str(),
	     stanza->to().full().c_str() ));
    if(stanza->to().full() == gateway->getFQDN()) {
      if(stanza->body() == "") return;
      std::string reply;
      if(stanza->body() == "users") {
	reply = "Current Users of this xfiregateway:\n";
	char buf[128];
	for(std::vector<User*>::iterator it = gateway->getUsers()->begin() ;
	    it != gateway->getUsers()->end() ; it++) {
	  sprintf(buf,"%s - %s - %d Buddies %s\n",
		  (*it)->jid.c_str(),
		  (*it)->name.c_str(),
		  (*it)->inroster.size(),
		  ((*it)->isOnline() ? "(Online)" : ""));
	  reply += buf;
	}
      } else if(stanza->body() == "uptime") {
	time_t uptime;
	time(&uptime);
	uptime -= gateway->startTime;
	int seconds = uptime % 60;
	uptime /= 60;
	int minutes = uptime % 60;
	uptime /= 60;
	int hours = uptime % 24;
	uptime /= 24;
	int days = uptime;
	char buf[128];
	sprintf(buf,"Current uptime is %d days, %d hours, %d minutes, %d seconds",days,hours,minutes,seconds);
	reply = buf;
      }
      if(!reply.empty()) {
	Tag *r = Stanza::createMessageStanza( stanza->from(), reply );
	r->addAttribute( "from",gateway->getFQDN());
	comp->send(r);
      }
    } else {
      User *user = gateway->getUserByJID( stanza->from().bare() );
      if(!user) {
	XINFO(( "Received message from someone not registered ?! (%s) \n",
		stanza->from().full().c_str() ));
      } else {
	user->receivedMessage( stanza );
      }
    }
  }



  void GatewayHandler::onConnect() {
    XINFO(( "onConnect\n" ));
    std::vector<User*> *users = gateway->getUsers();
    std::vector<User*>::iterator it = users->begin();
    while(it != users->end()) {
      (*it)->sendInitialPresences();
      it++;
    }
    gateway->writeUserFile();
  }
  void GatewayHandler::onDisconnect(ConnectionError error) {
    XINFO(( "onDisconnect\n" ));
  }
  void GatewayHandler::onResourceBindError(ResourceBindError error) {
    XINFO(( "onResourceBindError\n" ));
  }
  void GatewayHandler::onSessionCreateError(SessionCreateError error) {
    XINFO(( "onSessionCreateError\n" ));
  }
  bool GatewayHandler::onTLSConnect(const CertInfo &info) {
    XINFO(( "onTLSConnect\n" ));
    return true;
  }

};

