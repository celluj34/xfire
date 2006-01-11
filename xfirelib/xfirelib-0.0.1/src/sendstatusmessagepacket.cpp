#include "sendstatusmessagepacket.h"

#include <string.h>

namespace xfirelib {

  int SendStatusMessagePacket::getPacketContent(char *buf) {
    int index = 0;
    buf[index++] = 0x2e;
    buf[index++] = 0x01;
    buf[index++] = awaymsg.length();
    buf[index++] = 0x00;
    memcpy(buf+index,awaymsg.c_str(),awaymsg.length());
    index+=awaymsg.length();
    return index;
  }


};
