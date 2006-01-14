#ifndef __BUDDYLISTGAMESPACKET_H
#define __BUDDYLISTGAMESPACKET_H

#define XFIRE_BUDDYS_GAMES_ID 135

#include <vector>
#include <string>

#include "xfirepacketcontent.h"
#include "variablevalue.h"


namespace xfirelib {
  using namespace std;

  class BuddyListGamesPacket : public XFirePacketContent {
  public:
    BuddyListGamesPacket();

    XFirePacketContent* newPacket() { return new BuddyListGamesPacket(); }

    int getPacketId() { return XFIRE_BUDDYS_GAMES_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);


    vector<long> *gameids;
    vector<char *> *sids;
  };
};


#endif
