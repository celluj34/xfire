#include "sendacceptinvitationpacket.h"

#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {

  int SendAcceptInvitationPacket::getPacketContent(char *buf) {
    VariableValue val;
    val.setName( "name" );
    val.setValue( name.c_str() );
    int index = 0;
    index += val.writeName(buf, index);
    buf[index++] = 01;
    buf[index++] = name.length();
    buf[index++] = 00;
    index += val.writeValue(buf, index);

    return index;
  }


};
