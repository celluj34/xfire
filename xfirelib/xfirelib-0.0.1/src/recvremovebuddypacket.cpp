

#include "recvremovebuddypacket.h"
#include "variablevalue.h"

namespace xfirelib {

  void RecvRemoveBuddyPacket::parseContent(char *buf, int length, int numberOfAtts) {
    VariableValue val;
    int index = 0;
    index += val.readName(buf,index);
    index ++; // Ignore 02 ??
    index += val.readValue(buf,index,4);
    userid = val.getValueAsLong();
  }

};
