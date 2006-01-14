#ifndef __BUDDYLIST_H
#define __BUDDYLIST_H

#include <iostream>
#include <vector>

#include "buddylistnamespacket.h"
#include "buddylistgamespacket.h"
#include "buddylistonlinepacket.h"
#include "packetlistener.h"
#include "client.h"

namespace xfirelib {
  using namespace std;

  struct Client;
  struct BuddyListEntry;

  class BuddyList : public PacketListener {
  public:
    BuddyList(Client *client);
    ~BuddyList();

    vector <BuddyListEntry *> * getEntries() { return entries; }

    void receivedPacket(XFirePacket *packet);
    BuddyListEntry *getBuddyById(long userid);
    BuddyListEntry *getBuddyBySid(const char *sid);
    BuddyListEntry *getBuddyByName(string username);
  private:
    void initEntries(BuddyListNamesPacket* buddynames);
    void updateOnlineBuddies(BuddyListOnlinePacket* buddiesOnline);
    void updateBuddiesGame(BuddyListGamesPacket* buddiesGames);

    Client *client;
    vector <BuddyListEntry *> * entries;
  };


  class BuddyListEntry {
  public:
    BuddyListEntry();
    bool isOnline();

    void setSid(const char *sid);

    long userid;
    char sid[16];
    string nick;
    string username;
    string statusmsg;
    long game;
    long game2;
  };
};

#endif
