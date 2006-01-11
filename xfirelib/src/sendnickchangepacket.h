#ifndef __SENDNICKCHANGEPACKET_H
#define __SENDNICKCHANGEPACKET_H

#include "xfiresendpacketcontent.h"
#include <string>

#define XFIRE_SEND_NICKCHANGE_PACKET_ID 14;

namespace xfirelib {
  using namespace std;

  class SendNickChangePacket : public XFireSendPacketContent {
  public:
    XFirePacketContent *newPacket() { return new SendNickChangePacket; }

    int getPacketId() { return XFIRE_SEND_NICKCHANGE_PACKET_ID; }

    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 1; }
    int getPacketSize() { return 100; }

    string nick;
  };

};


#endif
