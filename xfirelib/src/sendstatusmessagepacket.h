#ifndef __SENDSTATUSMESSAGEPACKET_H
#define __SENDSTATUSMESSAGEPACKET_H

#include "xfiresendpacketcontent.h"
#include <string>

#define XFIRE_SEND_STATUSMESSAGE_PACKET_ID 32;

namespace xfirelib {
  using namespace std;

  class SendStatusMessagePacket : public XFireSendPacketContent {
  public:
    XFirePacketContent *newPacket() { return new SendStatusMessagePacket; }

    int getPacketId() { return XFIRE_SEND_STATUSMESSAGE_PACKET_ID; }

    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 1; }
    int getPacketSize() { return 100; }


    string awaymsg;
  };

};


#endif
