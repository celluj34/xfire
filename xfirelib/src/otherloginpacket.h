#ifndef __OTHERLOGINPACKET_H
#define __OTHERLOGINPACKET_H

#include "xfirepacketcontent.h"
#include "variablevalue.h"

#define XFIRE_OTHER_LOGIN 145

namespace xfirelib {

  class OtherLoginPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new OtherLoginPacket(); }

    int getPacketId() { return XFIRE_OTHER_LOGIN; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);
  };
};


#endif
