#ifndef __SENDREMOVEBUDDYPACKET_H
#define __SENDREMOVEBUDDYPACKET_H

#include "xfiresendpacketcontent.h"

#define XFIRE_SENDREMOVEBUDDYPACKET 9

namespace xfirelib {

  class SendRemoveBuddyPacket : public XFireSendPacketContent {
  public:
    int getPacketId() { return XFIRE_SENDREMOVEBUDDYPACKET; }

    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 1; }
    int getPacketSize() { return 100; }

    long userid;
  };

};

#endif
