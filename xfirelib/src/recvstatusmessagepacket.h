#ifndef __RECVMESSAGEPACKET_H
#define __RECVMESSAGEPACKET_H

#define XFIRE_RECV_STATUSMESSAGE_PACKET_ID 154

#include "xfirerecvpacketcontent.h"
#include <string>
#include <vector>

namespace xfirelib {

  class RecvStatusMessagePacket : public XFireRecvPacketContent {
  public:
    RecvStatusMessagePacket();
    XFirePacketContent *newPacket() { return new RecvStatusMessagePacket; }
    int getPacketId() { return XFIRE_RECV_STATUSMESSAGE_PACKET_ID; }

    void parseContent(char *buf, int length, int numberOfAtts);

    std::vector<char *> *sids;
    std::vector<std::string> *msgs;
  private:
    int readStrings(std::vector<std::string> *strings, char *buf, int index);
  };

};


#endif
