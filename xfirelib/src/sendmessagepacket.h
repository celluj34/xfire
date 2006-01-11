

#ifndef __SENDMESSAGEPACKET_H
#define __SENDMESSAGEPACKET_H



#include "xfiresendpacketcontent.h"
#include "variablevalue.h"
#include <string.h>
#include "client.h"

namespace xfirelib {

  class SendMessagePacket : public XFireSendPacketContent {
  public:
    SendMessagePacket() { }

    void init(Client *client, string username, string message);
    void setSid(const char *sid);

    XFirePacketContent* newPacket() { return new SendMessagePacket(); }

    int getPacketId() { return 2; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount() {return 2;};
    int getPacketSize() { return 300; };
    char sid[16];
    long imindex;
    std::string message;
  };

};

#endif
