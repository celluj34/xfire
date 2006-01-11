#ifndef __AUTHPACKET_H
#define __AUTHPACKET_H

#include "xfirepacketcontent.h"
#include "variablevalue.h"

#define XFIRE_PACKET_AUTH_ID 128

namespace xfirelib {

  class AuthPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new AuthPacket(); }


    int getPacketId() { return XFIRE_PACKET_AUTH_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);

    VariableValue *getSalt() { return salt; }
  private:
    VariableValue *salt;
  };
};


#endif
