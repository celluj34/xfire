#ifndef __RECVREMOVEBUDDYPACKET_H
#define __RECVREMOVEBUDDYPACKET_H

#include "xfirerecvpacketcontent.h"


#define XFIRE_RECVREMOVEBUDDYPACKET 139

namespace xfirelib {
  class RecvRemoveBuddyPacket : public XFireRecvPacketContent {
  public:
    int getPacketId() { return XFIRE_RECVREMOVEBUDDYPACKET; }

    XFirePacketContent *newPacket() { return new RecvRemoveBuddyPacket(); }
    void parseContent(char *buf, int length, int numberOfAtts);

    long userid;
  };

};


#endif
