#include "sendkeepalivepacket.h"
#include "variablevalue.h"
#include <string.h>

namespace xfirelib {

  int SendKeepAlivePacket::getPacketContent(char *buf) {
    int index = 0;

    VariableValue val;
    val.setName("value");

    index += val.writeName(buf,index);
    buf[index++] = 2;
    buf[index++] = 0;
    buf[index++] = 0;
    buf[index++] = 0;
    buf[index++] = 0;

    val.setName("stats");

    index += val.writeName(buf,index);
    buf[index++] = 4;
    buf[index++] = 2;
    buf[index++] = 0;
    buf[index++] = 0;

    return index;
  }


};
