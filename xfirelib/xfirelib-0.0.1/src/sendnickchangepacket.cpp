#include "sendnickchangepacket.h"
#include "variablevalue.h"
#include <string.h>

namespace xfirelib {
/*TODO: this packet is outdated, sniff with official client and change it to a correct implementation*/
  int SendNickChangePacket::getPacketContent(char *buf) {
    int index = 0;

    VariableValue val;
    val.setName("nick");
    val.setValue((char*)nick.c_str(),nick.size());

    index += val.writeName(buf,index);
    buf[index++] = 1;
    buf[index++] = nick.size();
    buf[index++] = 0;

    index += val.writeValue(buf,index);

    return index;
  }


};
