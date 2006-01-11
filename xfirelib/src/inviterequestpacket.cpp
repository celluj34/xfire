#include "inviterequestpacket.h"
#include "variablevalue.h"
#include <string>

namespace xfirelib {
  using namespace std;

  void InviteRequestPacket::parseContent(char *buf, int length, int numberOfAtts) {
    VariableValue val;
    int index = 0;
    index += val.readName(buf,index);
    index += 4; // Ignore 04 01 01 00
    index += val.readValue(buf,index,-1,1);
    name = string(val.getValue(),val.getValueLength());
    
    index += val.readName(buf,index);
    index += 4; // Ignore 04 01 01 00
    index += val.readValue(buf,index,-1,1);
    nick = string(val.getValue(),val.getValueLength());
    
    index += val.readName(buf,index);
    index += 4; // Ignore 04 01 01 00
    index += val.readValue(buf,index,-1,1);
    msg = string(val.getValue(),val.getValueLength());
  }

};
