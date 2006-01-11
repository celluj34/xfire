

#ifndef __LOGINPACKET_H
#define __LOGINPACKET_H

#include "xfirepacketcontent.h"

namespace xfirelib {
  class LoginPacket : public XFirePacketContent {
  public:
    LoginPacket();
    ~LoginPacket();
    
    XFirePacketContent* newPacket() { return new LoginPacket(); }

    int getPacketId() { return 1; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount();
    int getPacketSize();
    void parseContent(char *buf, int length, int numberOfAtts) { }

  private:
  };
};


#endif
