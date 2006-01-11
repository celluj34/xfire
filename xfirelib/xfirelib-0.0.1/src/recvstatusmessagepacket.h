#ifndef __RECVMESSAGEPACKET_H
#define __RECVMESSAGEPACKET_H

#define XFIRE_RECV_STATUSMESSAGE_PACKET_ID 154

#include "xfirerecvpacketcontent.h"
#include <string>

namespace xfirelib {

  class RecvStatusMessagePacket : public XFireRecvPacketContent {
  public:
    RecvStatusMessagePacket();
    XFirePacketContent *newPacket() { return new RecvStatusMessagePacket; }
    int getPacketId() { return XFIRE_RECV_STATUSMESSAGE_PACKET_ID; }

    void parseContent(char *buf, int length, int numberOfAtts);

    char sid[16];
    std::string msg;
  private:
  };

};


#endif
