

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

namespace xfiregateway {
  using namespace gloox;

  User::User(XFireGateway *gateway) : MonitoredObj("User") {
    client = NULL;
    this->gateway = gateway;
    this->comp = gateway->getComponent();
  }
  User::~User() {
    if(isOnline()) {
      {
	XDEBUG(( "Sending unavailable for all buddylist entries\n" ));
	xfirelib::BuddyList *buddyList = client->getBuddyList();
	vector<xfirelib::BuddyListEntry *> *entries = buddyList->getEntries();
	vector<xfirelib::BuddyListEntry *>::iterator it = entries->begin();
	while(it != entries->end()) {
	  if(isInRoster((*it)->username)) {
	    XDEBUG(( "Sending unavailable for %s\n", (*it)->username.c_str() ));
	    Stanza *stanza = Stanza::createPresenceStanza( JID(jid), "", PRESENCE_UNAVAILABLE );
	    stanza->addAttrib( "from", (*it)->username + "@" + gateway->getFQDN() );
	    comp->send( stanza );
	  }
	  it++;
	}
	XDEBUG(( "Done\n" ));
      }
      
      Stanza *stanza = Stanza::createPresenceStanza( JID(jid), "", PRESENCE_UNAVAILABLE );
      stanza->addAttrib( "from", gateway->getFQDN() );
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
    }
  }
  void User::preRemove() {
    for( vector<std::string>::iterator it = inroster.begin() ;
	 it != inroster.end() ; it++ ) {
      Tag *tag = new Tag("presence");
      tag->addAttrib( "to", jid );
      tag->addAttrib( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttrib( "type", "unavailable" );
      comp->send(tag);
      tag = new Tag("presence");
      tag->addAttrib( "to", jid );
      tag->addAttrib( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttrib( "type", "unsubscribe" );
      comp->send(tag);
      tag = new Tag("presence");
      tag->addAttrib( "to", jid );
      tag->addAttrib( "from", (*it) + "@" + gateway->getFQDN() );
      tag->addAttrib( "type", "unsubscribed" );
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
    tag->addAttrib( "type", "probe" );
    tag->addAttrib( "from", gateway->getFQDN() );
    tag->addAttrib( "to", jid );
    comp->send( tag );
  }
  void User::createPacketExtension( xfirelib::XFireGame *game, Tag *parent, std::string &awaymsg ) {
    XDEBUG(( "createPacketExtension( ... )\n" ));
    GOIMGame *g = (GOIMGame*)game;
    Tag *x = new Tag( "x" );
    x->addAttrib( "xmlns", "http://goim.sphene.net/gameStatus" );
    x->addAttrib( "game", g->info->goimid );
    x->addAttrib( "target", g->getTarget() );
    parent->addChild( x );
    awaymsg += "Playing: " + g->getGameName() + " at " + g->getTarget() + "  ";
  }
  void User::sendPresenceTagForBuddy(xfirelib::BuddyListEntry *buddy) {
    Tag *tag = new Tag("presence");
    tag->addAttrib( "to", jid );
    tag->addAttrib( "from", buddy->username + "@" + gateway->getFQDN() );
    if(!buddy->isOnline())
      tag->addAttrib( "type", "unavailable" );
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
    tag->addAttrib( "to", jid );
    tag->addAttrib( "from", gateway->getFQDN() );
    if(client) {
      
    } else {
      tag->addAttrib( "type", "unavailable" );
    }
    sleep(1);
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
	XERROR(( "WTF ?! client is NULL !!! HELP !!!\n" ));
	return;
      }
      if(stanza->subtype() == STANZA_S10N_SUBSCRIBED) {
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
      } else if(stanza->subtype() == STANZA_S10N_SUBSCRIBE) {
	if(client->getBuddyList()->getBuddyByName( stanza->to().username() )) {
	  Tag *tag = new Tag("presence");
	  tag->addAttrib( "to", stanza->from().bare() );
	  tag->addAttrib( "from", stanza->to().bare() );
	  tag->addAttrib( "type", "subscribed" );
	  comp->send( tag );
	} else {
	  XDEBUG(( "sending invite to username: %s\n",stanza->to().username().c_str() ));
	  xfirelib::InviteBuddyPacket invite;
	  invite.addInviteName( stanza->to().username(), "Pls add me :) (Default msg by GOIM xfire jabber gateway http://goim.us )" );
	  client->send( &invite );
	}
      } else if(stanza->subtype() == STANZA_S10N_UNSUBSCRIBE ||
		stanza->subtype() == STANZA_S10N_UNSUBSCRIBED) {
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
	tag->addAttrib( "to", jid );
	tag->addAttrib( "from", stanza->to().username() + "@" + gateway->getFQDN() );
	if(stanza->subtype() == STANZA_S10N_UNSUBSCRIBE) {
	  tag->addAttrib( "type", "unsubscribe" );
	} else {
	  tag->addAttrib( "type", "unsubscribed" );
	}
	comp->send( tag );
      }
      return;
    }
    if(stanza->show() == PRESENCE_UNAVAILABLE) {
      delete presences[ resource ];
      presences.erase( resource );
    } else if(stanza->subtype() == STANZA_PRES_AVAILABLE) {
      //if(stanza->show() != PRESENCE_AVAILABLE) return; // None of our business...
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
      XDEBUG(( "Just parsed presence... msg: %s / priority: %d\n", pres->statusmsg.c_str(), pres->priority ));
    }
    presenceChanged();
  }
  void User::receivedMessage(Stanza *stanza) {
    if(stanza->body() == "") return; // Ignore messages with empty bodies (maybe notification about typing ?)
    if(!client) {
      // TODO send error message back
      Stanza * reply = Stanza::createMessageStanza( stanza->from(), "Error while sending message: No Client connection !" );
      reply->addAttrib( "from", gateway->getFQDN() );
      comp->send(reply);
      return;
    }
    xfirelib::BuddyListEntry *entry = client->getBuddyList()->getBuddyByName( stanza->to().username() );
    if(!entry || !entry->isOnline()) {
      // TODO send error message
      Stanza * reply = Stanza::createMessageStanza( stanza->from(), "Error while sending message: User is not online (xfire does not support offline message sending) !" );
      reply->addAttrib( "from", gateway->getFQDN() );
      comp->send(reply);
      return;
    }
    xfirelib::SendMessagePacket packet;
    packet.setSid(entry->sid);
    packet.message = stanza->body();
    client->send( &packet );
  }

  void User::receivedPacket(xfirelib::XFirePacket *packet) {
    xfirelib::XFirePacketContent *content = packet->getContent();
    switch(content->getPacketId()) {
    case XFIRE_MESSAGE_ID: {
      xfirelib::MessagePacket *msg = (xfirelib::MessagePacket*)content;
      Stanza *stanza = Stanza::createMessageStanza( JID(jid), msg->getMessage() );
      stanza->addAttrib( "from", client->getBuddyList()->getBuddyBySid( msg->getSid() )->username + "@" + gateway->getFQDN() );
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
	  tag->addAttrib( "type", "subscribe" );
	  tag->addAttrib( "from", (*it)->username + "@" + gateway->getFQDN() );
	  tag->addAttrib( "to", jid );
	  comp->send( tag );
	}
	/*
	Tag *tag = new Tag("presence");
	tag->addAttrib( "type", "probe" );
	tag->addAttrib( "from", (*it)->username + "@" + gateway->getFQDN() );
	tag->addAttrib( "to", jid );
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
	sendPresenceTagForBuddy( buddy );
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
        sendPresenceTagForBuddy( buddy );
        it++;
      }
      break;
    }
    case XFIRE_PACKET_INVITE_REQUEST_PACKET: {
      xfirelib::InviteRequestPacket *invite = (xfirelib::InviteRequestPacket*)content;
      Tag *tag = new Tag("presence");
      tag->addAttrib( "from", invite->name + "@" + gateway->getFQDN() );
      tag->addAttrib( "to", jid );
      tag->addAttrib( "type", "subscribe" );
      comp->send(tag);
      break;
    }
    case XFIRE_RECVREMOVEBUDDYPACKET: {
      xfirelib::RecvRemoveBuddyPacket *p = (xfirelib::RecvRemoveBuddyPacket*)content;
      Tag *tag = new Tag("presence");
      tag->addAttrib( "from", p->username + "@" + gateway->getFQDN() );
      tag->addAttrib( "to", jid );
      tag->addAttrib( "type", "unsubscribed" );
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
    }
  }
  void User::presenceChanged() {
    UserPresence *pres = getPresence();
    if(pres) {
      XDEBUG(( "Got Presence... status: %d\n", pres->status ));
      if(!client) {
	client = new xfirelib::Client();
	client->connect( name, password );
	client->addPacketListener(this);
	//client->setGameResolver( new xfirelib::DummyXFireGameResolver() );
	client->setGameResolver( gateway->gameXML );
      }
      xfirelib::SendStatusMessagePacket packet;
      if(pres->status != PRESENCE_AVAILABLE)
	packet.awaymsg = "away: " + pres->statusmsg;
      client->send( &packet );
    } else {
      if(client) {
	XDEBUG(( "disconnecting client\n" ));
	client->disconnect();
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
