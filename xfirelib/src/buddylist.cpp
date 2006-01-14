
#include <iostream>
#include <vector>
#include "buddylist.h"
#include "buddylistonlinepacket.h"
#include "buddylistgamespacket.h"
#include "recvremovebuddypacket.h"
#include "recvstatusmessagepacket.h"
#include "xdebug.h"


namespace xfirelib {
  using namespace std;

  BuddyList::BuddyList(Client *client) {
    entries = new vector<BuddyListEntry *>;
    this->client = client;
    this->client->addPacketListener( this );
  }
  BuddyList::~BuddyList() {
    delete entries;
  }

  BuddyListEntry *BuddyList::getBuddyById(long userid) {
    for(int i = 0 ; i < entries->size() ; i++) {
      BuddyListEntry *entry = entries->at(i);
      if(entry->userid == userid)
	return entry;
    }
    return 0;
  }

  BuddyListEntry *BuddyList::getBuddyByName(string username) {
    for(int i = 0 ; i < entries->size() ; i++) {
      BuddyListEntry *entry = entries->at(i);
      if(entry->username == username)
	return entry;
    }
    return 0;
  }

  BuddyListEntry *BuddyList::getBuddyBySid(const char *sid) {
    for(int i = 0 ; i < entries->size() ; i++) {
      BuddyListEntry *entry = entries->at(i);
      if(memcmp((void *)sid,(void *)entry->sid,16) == 0)
	return entry;
    }
    return 0;
  }


  void BuddyList::initEntries(BuddyListNamesPacket *buddyNames) {
    for(int i = 0 ; i < buddyNames->usernames->size() ; i++) {
      BuddyListEntry *entry = new BuddyListEntry;
      entry->username = buddyNames->usernames->at(i);
      entry->userid = buddyNames->userids->at(i);
      entry->nick = buddyNames->nicks->at(i);
      entries->push_back(entry);
    }
  }
  void BuddyList::updateOnlineBuddies(BuddyListOnlinePacket* buddiesOnline) {
    for(int i = 0 ; i < buddiesOnline->userids->size() ; i++) {
      BuddyListEntry *entry = getBuddyById( buddiesOnline->userids->at(i) );
        if(entry){
         entry->setSid( buddiesOnline->sids->at(i) );
        }else{
         XERROR(("Could not find buddy with this sid!\n"));
        }
    }
  }

  void BuddyList::updateBuddiesGame(BuddyListGamesPacket* buddiesGames) {
    for(int i = 0 ; i < buddiesGames->sids->size() ; i++) {
      BuddyListEntry *entry = getBuddyBySid( buddiesGames->sids->at(i) );
      if(entry){
      entry->game = buddiesGames->gameids->at(i);
      }else{
        XERROR(("Could not find buddy with this sid!\n"));
      }
    }
  }

  void BuddyList::receivedPacket(XFirePacket *packet) {
    XFirePacketContent *content = packet->getContent();
    if(content == 0) return;
    XDEBUG(( "hmm... %d\n", content->getPacketId() ));
    switch(content->getPacketId()) {
    case XFIRE_BUDDYS_NAMES_ID: {
      XINFO(( "Received Buddy List..\n" ));
      this->initEntries( (BuddyListNamesPacket*)content );
      break;
    }
    case XFIRE_BUDDYS_ONLINE_ID: {
      XINFO(( "Received Buddy Online Packet..\n" ));
      this->updateOnlineBuddies( (BuddyListOnlinePacket *)content );
      break;
    }
    case XFIRE_BUDDYS_GAMES_ID: {
      XINFO(( "Recieved the game a buddy is playing..\n" ));
      this->updateBuddiesGame( (BuddyListGamesPacket *)content );
    }
    case XFIRE_RECVREMOVEBUDDYPACKET: {
      RecvRemoveBuddyPacket *p = (RecvRemoveBuddyPacket*)content;
      XDEBUG(( "Buddy was removed from contact list (userid: %d)\n", p->userid ));
      std::vector<BuddyListEntry *>::iterator i = entries->begin();
      while( i != entries->end() ) {
	if((*i)->userid == p->userid) {
	  BuddyListEntry *buddy = *i;
	  XINFO(( "%s (%s) was removed from BuddyList.\n", buddy->username.c_str(), buddy->nick.c_str() ));
	  entries->erase(i);
	  // i.erase();
	  break; // we are done.
	}
	++i;
      }
      break;
    }
    case XFIRE_RECV_STATUSMESSAGE_PACKET_ID: {
      RecvStatusMessagePacket *status = (RecvStatusMessagePacket*) content;

     for(int i = 0 ; i < status->sids->size() ; i++) {
        BuddyListEntry *entry = getBuddyBySid( status->sids->at(i) );
        if(entry == NULL) {
            XERROR(( "No such Entry - Got StatusMessage from someone who is not in the buddylist ??\n" ));
            return;
        }
        entry->statusmsg = status->msgs->at(i).c_str();
    }

      break;
    }
    }
  }


  BuddyListEntry::BuddyListEntry() {
    memset(sid,0,16);
    statusmsg = std::string();
  }
  bool BuddyListEntry::isOnline() {
    for(int i = 0 ; i < 16 ; i++) {
      if(sid[i]) return true;
    }
    return false;
  }
  void BuddyListEntry::setSid(const char *sid) {
    memcpy(this->sid,sid,16);
  }
};

