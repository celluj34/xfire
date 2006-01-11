

#ifndef __CLIENTVERSIONPACKET_H
#define __CLIENTVERSIONPACKET_H

#include "xfirepacketcontent.h"

namespace xfirelib {

  class ClientVersionPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new ClientVersionPacket(); }

    int getPacketId() { return 3; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount();
    int getPacketSize() { return 1024; };
    void setProtocolVersion(int version);
    void parseContent(char *buf, int length, int numberOfAtts) { };
  private:
    int length;
    int version;
  };

};

#endif
