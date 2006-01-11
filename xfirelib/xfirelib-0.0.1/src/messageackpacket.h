#ifndef __MESSAGEACKPACKET_H
#define __MESSAGEACKPACKET_H

#include "xfirepacketcontent.h"

#define XFIRE_MESSAGE_ACK_ID 2

namespace xfirelib {

  class MessageACKPacket : public XFirePacketContent {
  public:
    MessageACKPacket();
    ~MessageACKPacket();

    XFirePacketContent* newPacket() { return new MessageACKPacket(); }


    int getPacketId() { return XFIRE_MESSAGE_ACK_ID; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 2; }
    int getPacketSize() { return 100; };
    void parseContent(char *buf, int length, int numberOfAtts);

    char sid[16];
    long imindex;

  private:
  };

};


#endif

