#ifndef __LOGINSUCCESSPACKET_H
#define __LOGINSUCCESSPACKET_H

#include "xfirepacketcontent.h"
#include "variablevalue.h"

#define XFIRE_LOGIN_SUCCESS_ID 130

namespace xfirelib {

  class LoginSuccessPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new LoginSuccessPacket(); }

    int getPacketId() { return XFIRE_LOGIN_SUCCESS_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);
  };

};


#endif
