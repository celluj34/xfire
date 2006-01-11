#include "sendremovebuddypacket.h"

#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {

  int SendRemoveBuddyPacket::getPacketContent(char *buf) {
    int index = 0;
    VariableValue val;
    val.setName("userid");
    XDEBUG(( "Creating packet to remove userid %d\n", userid ));
    index += val.writeName(buf,index);
    val.setValueFromLong(userid,4);
    buf[index++] = 2;
    index += val.writeValue(buf,index);

    return index;
  }

};

