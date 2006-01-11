#ifndef __BUDDYLISTONLINEPACKET_H
#define __BUDDYLISTONLINEPACKET_H

#include <vector>

#include "xfirepacketcontent.h"
#include "variablevalue.h"


#define XFIRE_BUDDYS_ONLINE_ID 132

namespace xfirelib {
using namespace std;
  class BuddyListOnlinePacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new BuddyListOnlinePacket(); }


    int getPacketId() { return XFIRE_BUDDYS_ONLINE_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);

    vector<long> *userids;
    vector<char *> *sids;
  };
};


#endif
