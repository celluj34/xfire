

#ifndef __CLIENTINFORMATIONPACKET_H
#define __CLIENTINFORMATIONPACKET_H

#include "xfirepacketcontent.h"

namespace xfirelib {

  class ClientInformationPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new ClientInformationPacket(); }

    int getPacketId() { return 18; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount();
    int getPacketSize() { return 46; };
    void parseContent(char *buf, int length, int numberOfAtts) { };
  private:
    int length;
  };

};

#endif
