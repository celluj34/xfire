#ifndef __SENDKEEPALIVEPACKET_H
#define __SENDKEEPALIVEPACKET_H

#include "xfiresendpacketcontent.h"
#include <string>

#define XFIRE_SEND_KEEPALIVE_PACKET_ID 13;

namespace xfirelib {
  using namespace std;

  class SendKeepAlivePacket : public XFireSendPacketContent {
  public:
    int getPacketId() { return XFIRE_SEND_KEEPALIVE_PACKET_ID; }

    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 2; }
    int getPacketSize() { return 26; }

  };

};


#endif
