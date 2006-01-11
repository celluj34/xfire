#ifndef __LOGINFAILEDPACKET_H
#define __LOGINFAILEDPACKET_H

#include "xfirepacketcontent.h"
#include "variablevalue.h"

#define XFIRE_LOGIN_FAILED_ID 129

namespace xfirelib {

  class LoginFailedPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new LoginFailedPacket(); }


    int getPacketId() { return XFIRE_LOGIN_FAILED_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);
  };
};


#endif
