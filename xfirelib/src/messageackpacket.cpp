

#include "messageackpacket.h"
#include "xdebug.h"
#include "string.h"
#include "variablevalue.h"
#include <iostream>

namespace xfirelib {

  MessageACKPacket::MessageACKPacket() {
    memset( sid, 0, 16 );
    imindex = -1;
  }
  MessageACKPacket::~MessageACKPacket() {
  }

  int MessageACKPacket::getPacketContent(char *buf) {
    int index = 0;
    VariableValue val;
    val.setName("sid");
    val.setValue(sid,16);

    index += val.writeName(buf,index);
    buf[index++] = 3;
    index += val.writeValue(buf,index);

    val.setName("peermsg");
    index += val.writeName(buf,index);
    buf[index++] = 5;
    buf[index++] = 2;

    val.setName("msgtype");
    val.setValueFromLong(1,4);
    index += val.writeName(buf,index);
    buf[index++] = 2;
    index += val.writeValue(buf,index);

    val.setName("imindex");
    val.setValueFromLong(imindex,4);
    index += val.writeName(buf,index);
    buf[index++] = 02;
    index += val.writeValue(buf,index);

    return index;
  }

  void MessageACKPacket::parseContent(char *buf, int length, int numberOfAtts) {
  }


};

