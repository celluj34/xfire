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



#include "user.h"
#include <xfirelib/xdebug.h>
#include <xfirelib/sendstatusmessagepacket.h>
#include <xfirelib/sendmessagepacket.h>
#include <xfirelib/messagepacket.h>

#include <xfirelib/sendacceptinvitationpacket.h>
#include <xfirelib/invitebuddypacket.h>
#include <xfirelib/inviterequestpacket.h>

#include <xfirelib/buddylist.h>
#include <xfirelib/buddylistnamespacket.h>
#include <xfirelib/buddylistonlinepacket.h>
#include <xfirelib/recvstatusmessagepacket.h>
#include <xfirelib/sendremovebuddypacket.h>
#include <xfirelib/senddenyinvitationpacket.h>
#include <xfirelib/recvremovebuddypacket.h>
#include <xfirelib/buddylistgamespacket.h>
#include <xfirelib/dummyxfiregameresolver.h>
#include <xfirelib/sendgamestatuspacket.h>
#include <xfirelib/sendgamestatus2packet.h>
#include <xfirelib/otherloginpacket.h>

namespace xfiregateway {
  using namespace gloox;

  User::User(XFireGateway *gateway) : MonitoredObj("User") {
    client = NULL;
    this->gateway = gateway;
    this->comp = gateway->getComponent();
    this->lastsentgame = 0;
    this->lastsentgame2 = 0;
  }
  User::~User() {
    if(isOnline()) {
      {
	sendUnavailableForAllBuddies();
      }
      
      Stanza *stanza = Stanza::createPresenceStanza( JID(jid), "", PresenceUnavailable );
      stanza->addAttribute( "from", gateway->getFQDN() );
      comp->send( stanza );

      UserPresenceMap::iterator it = presences.begin();
      while(it != presences.end()) {
	delete it->second;
	it++;
      }
    }
    if(client) {
      XDEBUG(( "client->disconnect()\n" ));
      client->disconnect();
      delete client;
      client = NULL;
      lastsentgame = 0;
      lastsentgame2 = 0;
      XDEBUG(( "done client->disconnect() ....\n" ));
    }
  }
  void User::sendUnavailableForAllBuddies() {
    XDEBUG(( "Sending unavailable for all buddylist entries\n" ));
    xfirelib::BuddyList *buddyList = client->getBuddyList();
    vector<xfirelib::BuddyListEntry *> *entries = buddyList->getEntries();
    vector<xfirelib::BuddyListEntry *>::iterator it = entries->begin();
    while(it != entries->end()) {
      if(isInRoster((*it)->username)) {
	XDEBUG(( "Sending unavailable for %s\n", (*it)->username.c_str() ));
	Stanza *stanza = Stanza::createPresenceStanza( JID(jid), "", PresenceUnavailable );
	stanza->addAttribute( "from", (*it)->username + "@" + gateway->getFQDN() );
	comp->send( stanza );
      }
      it++;
    }
    XDEBUG(( "Done\n" ));
  }
  void User::preRemove() {
    for( vector<std::string>::iterator it = inroster.begin() ;
	 it != inroster.end() ; it++ ) {
      Tag *tag = new Tag("presence");
      tag->addAttribute( "to", jid );
      tag->addAttribute( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttribute( "type", "unavailable" );
      comp->send(tag);
      tag = new Tag("presence");
      tag->addAttribute( "to", jid );
      tag->addAttribute( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttribute( "type", "unsubscribe" );
      comp->send(tag);
      tag = new Tag("presence");
      tag->addAttribute( "to", jid );
      tag->addAttribute( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttribute( "type", "unsubscribed" );
      comp->send(tag);
    }
  }
  bool User::isInRoster(std::string username) {
    vector<std::string>::iterator j = inroster.begin();
    bool isinroster = false;
    while(!isinroster && j != inroster.end()) {
      if((*j) == username)
	isinroster = true;
      j++;
    }
    return isinroster;
  }
  void User::sendInitialPresences() {
    Tag *tag = new Tag("presence");
    tag->addAttribute( "type", "probe" );
    tag->addAttribute( "from", gateway->getFQDN() );
    tag->addAttribute( "to", jid );
    comp->send( tag );
  }
  void User::createPacketExtension( xfirelib::XFireGame *game, Tag *parent, std::string &awaymsg ) {
    XDEBUG(( "createPacketExtension( ... )\n" ));
    GOIMGame *g = (GOIMGame*)game;
    Tag *x = new Tag( "x" );
    x->addAttribute( "xmlns", "http://goim.sphene.net/gameStatus" );
    x->addAttribute( "game", g->info->goimid );
    x->addAttribute( "target", g->getTarget() );
    parent->addChild( x );
    awaymsg += "Playing: " + g->getGameName() + " at " + g->getTarget() + "  ";
  }
  void User::sendPresenceTagForBuddy(xfirelib::BuddyListEntry *buddy) {
    Tag *tag = new Tag("presence");
    tag->addAttribute( "to", jid );
    std::string from;
    from.append(buddy->username);
    from.append("@");
    from.append(gateway->getFQDN());
    tag->addAttribute( "from", from );
    if(!buddy->isOnline())
      tag->addAttribute( "type", "unavailable" );
    else {
      if(buddy->statusmsg != "") {
	tag->addChild( new Tag("show", "away" ) );
	tag->addChild( new Tag("status", buddy->statusmsg ) );
      }
      std::string ownawaymsg;
      if(buddy->gameObj)
	createPacketExtension( buddy->gameObj, tag, ownawaymsg );
      if(buddy->game2Obj)
	createPacketExtension( buddy->game2Obj, tag, ownawaymsg );
      if(buddy->statusmsg == "" && !ownawaymsg.empty() ) {
	tag->addChild( new Tag("show", "away" ) );
        tag->addChild( new Tag("status", ownawaymsg ) );
      }
    }
    comp->send( tag );
  }
  void User::mirrorPresence() {
    XDEBUG(( "mirrorPresence\n" ));
    Tag *tag = new Tag("presence");
    tag->addAttribute( "to", jid );
    tag->addAttribute( "from", gateway->getFQDN() );
    if(client) {
      
    } else {
      tag->addAttribute( "type", "unavailable" );
    }
    //sleep(1);
    XDEBUG(( "mirrorPresence sending tag ..\n" ));
    comp->send( tag );
    XDEBUG(( "mirrorPresence end\n" ));
  }
  void User::receivedPresence(Stanza *stanza) {
    //std::map<std::string, UserPresence *>::iterator it = stanza->find( stanza->from().resource() );
    std::string resource = stanza->from().resource();
    XDEBUG(( "Presence for %s\n", stanza->to().full().c_str() ));
    if(stanza->to().username() != "") {
      if(!client) {
	XDEBUG(( "WTF ?! client is NULL !!! HELP !!!\n" ));
	return;
      }
      if(stanza->subtype() == StanzaS10nSubscribed) {
	if(!isInRoster( stanza->to().username() ) ) {
	  XDEBUG(( "Recevied SUBSCRIBED ... adding to roster %s\n", stanza->to().username().c_str() ));
	  xfirelib::BuddyListEntry *buddy = client->getBuddyList()->getBuddyByName( stanza->to().username() );
	  inroster.push_back( stanza->to().username() );
	  if(!buddy) {
	    xfirelib::SendAcceptInvitationPacket accept;
	    accept.name = stanza->to().username();
	    client->send( &accept );
	  } else {
	    sendPresenceTagForBuddy( buddy );
	  }
	}
      } else if(stanza->subtype() == StanzaS10nSubscribe) {
	if(client->getBuddyList()->getBuddyByName( stanza->to().username() )) {
	  Tag *tag = new Tag("presence");
	  tag->addAttribute( "to", stanza->from().bare() );
	  tag->addAttribute( "from", stanza->to().bare() );
	  tag->addAttribute( "type", "subscribed" );
	  comp->send( tag );
	} else {
	  XDEBUG(( "sending invite to username: %s\n",stanza->to().username().c_str() ));
	  xfirelib::InviteBuddyPacket invite;
	  invite.addInviteName( stanza->to().username(), "Pls add me :) (Default msg by GOIM xfire jabber gateway http://goim.us )" );
	  client->send( &invite );
	}
      } else if(stanza->subtype() == StanzaS10nUnsubscribe ||
		stanza->subtype() == StanzaS10nUnsubscribed) {
	xfirelib::BuddyListEntry *entry = client->getBuddyList()->getBuddyByName(stanza->to().username());
	if(entry) {
	  xfirelib::SendRemoveBuddyPacket removeBuddy;
	  removeBuddy.userid = entry->userid;
	  client->send( &removeBuddy );
	} else {
	  xfirelib::SendDenyInvitationPacket deny;
	  deny.name = stanza->to().username();
	  client->send( &deny );
	}
	Tag *tag = new Tag("presence");
	tag->addAttribute( "to", jid );
	tag->addAttribute( "from", stanza->to().username() + "@" + gateway->getFQDN() );
	if(stanza->subtype() == StanzaS10nUnsubscribe) {
	  tag->addAttribute( "type", "unsubscribe" );
	} else {
	  tag->addAttribute( "type", "unsubscribed" );
	}
	comp->send( tag );
      }
      return;
    }
    if(stanza->show() == PresenceUnavailable) {
      delete presences[ resource ];
      presences.erase( resource );
    } else if(stanza->subtype() == StanzaPresenceAvailable) {
      //if(stanza->show() != PresenceAvailable) return; // None of our business...
      //presence.find
      UserPresence *pres;
      if(presences.count( resource ) < 1) {
	pres = new UserPresence();
	pres->resource = resource;
	presences[ resource ] = pres;
      } else {
	pres = presences[ resource ];
      }
      pres->status = stanza->show();
      pres->statusmsg = stanza->status();
      Tag *prioritytag = stanza->findChild( "priority" );
      if(prioritytag)
	pres->priority = atoi(prioritytag->cdata().c_str());
      else
	XERROR(( "No priority in presence ?!\n" ));
      Tag::TagList taglist = stanza->children();
      for(Tag::TagList::iterator it = taglist.begin() ;
	  it != taglist.end() ; it++) {
	if( (*it)->hasAttribute( "xmlns", "http://goim.sphene.net/gameStatus" ) ) {
	  std::string gameid = (*it)->findAttribute( "game" );
	  std::string target = (*it)->findAttribute( "target" );
	  XDEBUG(( "Received a game presence for game %s to target %s\n", gameid.c_str(), target.c_str() ));
	  GOIMGameInfo *gameInfo = gateway->gameXML->getGameInfoByGOIMId( gameid );
	  GOIMGame *game = new GOIMGame(gameInfo);
	  sscanf(target.c_str(),"%d.%d.%d.%d:%d",
		 &game->ip[0],
		 &game->ip[1],
		 &game->ip[2],
		 &game->ip[3],&game->port);
	  XDEBUG(( "Parsed target: %s\n", game->getTarget().c_str() ));
	  pres->games.push_back( game );
	}
      }
      XDEBUG(( "Just parsed presence... msg: %s / priority: %d\n", pres->statusmsg.c_str(), pres->priority ));
    }
    presenceChanged();
  }
  void User::receivedMessage(Stanza *stanza) {
    if(stanza->body() == "") return; // Ignore messages with empty bodies (maybe notification about typing ?)
    if(!client) {
      // TODO send error message back
      Stanza * reply = Stanza::createMessageStanza( stanza->from(), "Error while sending message: No Client connection !" );
      reply->addAttribute( "from", gateway->getFQDN() );
      comp->send(reply);
      return;
    }
    xfirelib::BuddyListEntry *entry = client->getBuddyList()->getBuddyByName( stanza->to().username() );
    if(!entry || !entry->isOnline()) {
      // TODO send error message
      Stanza * reply = Stanza::createMessageStanza( stanza->from(), "Error while sending message: User is not online (xfire does not support offline message sending) !" );
      reply->addAttribute( "from", gateway->getFQDN() );
      comp->send(reply);
      return;
    }
    xfirelib::SendMessagePacket *packet = new xfirelib::SendMessagePacket();
    XDEBUG(("setting sid ...\n"));
    packet->setSid(entry->sid);
    XDEBUG(("setting body ...\n"));
    packet->message = stanza->body();
    XDEBUG(("sending packet ...\n"));
    client->send( packet );
    XDEBUG(("done...\n"));
    delete packet;
  }

  void User::receivedPacket(xfirelib::XFirePacket *packet) {
    xfirelib::XFirePacketContent *content = packet->getContent();
    switch(content->getPacketId()) {
    case XFIRE_MESSAGE_ID: {
      xfirelib::MessagePacket *msg = (xfirelib::MessagePacket*)content;
      Stanza *stanza = Stanza::createMessageStanza( JID(jid), msg->getMessage() );
      stanza->addAttribute( "from", client->getBuddyList()->getBuddyBySid( msg->getSid() )->username + "@" + gateway->getFQDN() );
      comp->send(stanza);
      break;
    }
    case XFIRE_BUDDYS_NAMES_ID: {
      xfirelib::BuddyList *buddyList = client->getBuddyList();
      vector<xfirelib::BuddyListEntry *> *entries = buddyList->getEntries();
      vector<xfirelib::BuddyListEntry *>::iterator it = entries->begin();
      while(it != entries->end()) {
	if(!isInRoster((*it)->username)) {
	  Tag *tag = new Tag("presence");
	  tag->addAttribute( "type", "subscribe" );
	  tag->addAttribute( "from", (*it)->username + "@" + gateway->getFQDN() );
	  tag->addAttribute( "to", jid );
	  comp->send( tag );
	}
	/*
	Tag *tag = new Tag("presence");
	tag->addAttribute( "type", "probe" );
	tag->addAttribute( "from", (*it)->username + "@" + gateway->getFQDN() );
	tag->addAttribute( "to", jid );
	comp->send( tag );
	*/
	it++;
      }
      break;
    }
    case XFIRE_BUDDYS_ONLINE_ID: {
      xfirelib::BuddyListOnlinePacket *packet = (xfirelib::BuddyListOnlinePacket *)content;
      xfirelib::BuddyList *buddyList = client->getBuddyList();
      vector<long>::iterator it = packet->userids->begin();
      while(it != packet->userids->end()) {
	xfirelib::BuddyListEntry *buddy = buddyList->getBuddyById( (*it) );
	if(buddy) {
	  sendPresenceTagForBuddy( buddy );
	} else {
	  XERROR(( "Unable to find buddy in buddylist but received buddys online packet ?!\n" ));
	}
	it++;
      }
      break;
    }
    case XFIRE_RECV_STATUSMESSAGE_PACKET_ID: {
      xfirelib::RecvStatusMessagePacket *packet = (xfirelib::RecvStatusMessagePacket*) content;
      xfirelib::BuddyList *buddyList = client->getBuddyList();
      vector<char *>::iterator it = packet->sids->begin();
      while(it != packet->sids->end()) {
	xfirelib::BuddyListEntry *buddy = buddyList->getBuddyBySid( (*it) );
	if(buddy) {
	  sendPresenceTagForBuddy( buddy );
	} else {
	  XDEBUG(( "Received presence packet from someone not in my buddylist ?!\n" ));
	}
        it++;
      }
      break;
    }
    case XFIRE_PACKET_INVITE_REQUEST_PACKET: {
      xfirelib::InviteRequestPacket *invite = (xfirelib::InviteRequestPacket*)content;
      Tag *tag = new Tag("presence");
      tag->addAttribute( "from", invite->name + "@" + gateway->getFQDN() );
      tag->addAttribute( "to", jid );
      tag->addAttribute( "type", "subscribe" );
      comp->send(tag);
      break;
    }
    case XFIRE_RECVREMOVEBUDDYPACKET: {
      xfirelib::RecvRemoveBuddyPacket *p = (xfirelib::RecvRemoveBuddyPacket*)content;
      Tag *tag = new Tag("presence");
      tag->addAttribute( "from", p->username + "@" + gateway->getFQDN() );
      tag->addAttribute( "to", jid );
      tag->addAttribute( "type", "unsubscribed" );
      comp->send(tag);
      std::vector<std::string>::iterator it = inroster.begin();
      while(it != inroster.end()) {
	if((*it) == p->username) {
	  inroster.erase(it);
	  break;
	}
	it++;
      }
      break;
    }
    case XFIRE_BUDDYS_GAMES_ID: {
      xfirelib::BuddyListGamesPacket *p = (xfirelib::BuddyListGamesPacket*)content;
      for( std::vector<char *>::iterator it = p->sids->begin() ;
	   it != p->sids->end() ; it++ ) {
	xfirelib::BuddyListEntry *entry = client->getBuddyList()->getBuddyBySid( (*it) );
	if(entry) {
	  sendPresenceTagForBuddy( entry );
	}
      }
      break;
    }
    case XFIRE_OTHER_LOGIN: {
      sendUnavailableForAllBuddies();
      delete client;
      client = NULL;
      Tag *r = Stanza::createMessageStanza( JID(jid), "Someone else logged into the same account. Forced to disconnect." );
      r->addAttribute( "from",gateway->getFQDN());
      comp->send(r);
      r = Stanza::createPresenceStanza( JID(jid), "", PresenceUnavailable );
      r->addAttribute( "from",gateway->getFQDN());
      comp->send(r);
      break;
    }
    }
  }
  void User::presenceChanged() {
    UserPresence *pres = getPresence();
    if(pres) {
      XDEBUG(( "Got Presence... status: %d\n", pres->status ));
      if(!client) {
	client = new xfirelib::Client();
	mirrorPresence();
	client->connect( name, password );
	client->addPacketListener(this);
	//client->setGameResolver( new xfirelib::DummyXFireGameResolver() );
	client->setGameResolver( gateway->gameXML );
      }
      xfirelib::SendStatusMessagePacket packet;
      if(pres->status != PresenceAvailable)
	packet.awaymsg = "away: " + pres->statusmsg;
      client->send( &packet );

      // TODO .. check all resources of joined games.
      bool sentfirst = false;  // XFire only supports 1 game / 1 voip ..
      bool sentsecond = false; // so remember which we've sent
      for(std::vector<GOIMGame *>::iterator it = pres->games.begin() ;
	  it != pres->games.end() && (sentfirst == false || sentsecond == false) ; it++) {
	xfirelib::SendGameStatusPacket *packet = NULL;
	if((*it)->info->changestatus) {
	  if(!sentfirst) {
	    sentfirst = true;
	    if(lastsentgame != (*it)->info->xfireid) {
	      lastsentgame = (*it)->info->xfireid;
	      packet = new xfirelib::SendGameStatusPacket();
	    }
	  }
	} else {
	  if(!sentsecond) {
	    sentsecond = true;
	    if(lastsentgame2 != (*it)->info->xfireid) {
	      lastsentgame2 = (*it)->info->xfireid;
	      packet = new xfirelib::SendGameStatus2Packet();
	    }
	  }
	}
	if(packet) {
	  packet->gameid = (*it)->info->xfireid;
	  memcpy(packet->ip,(*it)->ip,4);
	  packet->port = (*it)->port;
	  client->send( packet );
	  delete packet;
	}
      }
      if(!sentfirst && lastsentgame > 0) {
	xfirelib::SendGameStatusPacket packet;
	client->send(&packet);
      }
      if(!sentsecond && lastsentgame2 > 0) {
	xfirelib::SendGameStatus2Packet packet;
	client->send(&packet);
      }
    } else {
      if(client) {
	XDEBUG(( "disconnecting client\n" ));
	sendUnavailableForAllBuddies();
	client->disconnect();
	delete client;
	client = NULL;
      }
    }
    mirrorPresence();
  }



  UserPresence *User::getPresence() {
    if(presences.size() < 1) return NULL;
    UserPresenceMap::iterator i = presences.begin();
    UserPresence *currentpres = 0;
    UserPresence *pres;
    while(i != presences.end()) {
      pres = i->second;
      if(!currentpres || pres->priority > currentpres->priority)
	currentpres = pres;

      i++;
    }
    return pres;
  }

  UserPresence::UserPresence() : MonitoredObj( "UserPresence" ) {
  }

  UserPresence::UserPresence(int status, std::string statusmsg, std::string resource, int priority) : MonitoredObj( "UserPresence") {
    this->status = status;
    this->statusmsg = statusmsg;
    this->resource = resource;
    this->priority = priority;
  }
  
  UserPresence::~UserPresence() {
  }
};
