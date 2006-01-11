#include "recvstatusmessagepacket.h"

#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {
  RecvStatusMessagePacket::RecvStatusMessagePacket() {
    msg = std::string();
  }

  void RecvStatusMessagePacket::parseContent(char *buf, int length, int numberOfAtts) {
    int index = 0;
    VariableValue val;
    index += val.readName(buf,index);
    index ++; // ignore 03 ??
    index ++; // ignore 01 ??
    index ++; // Ignore 00 ??
    memcpy(sid,buf+index,16);
    index += 16;
    index += val.readName(buf,index);
    index ++; // Ignore 04 ??
    index ++; // Ignore 01 ?? (length of length ??? TODO !!)
    index += val.readValue(buf,index,-1,1);
    msg = std::string(val.getValue(),0,val.getValueLength());
  }

};

