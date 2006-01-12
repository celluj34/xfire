#include "recvstatusmessagepacket.h"

#include "variablevalue.h"
#include "xdebug.h"
#include <vector>
#include <string>

namespace xfirelib {
  RecvStatusMessagePacket::RecvStatusMessagePacket() {
  }

  void RecvStatusMessagePacket::parseContent(char *buf, int length, int numberOfAtts) {
    int index = 0;
    int numberOfIds = 0;
    VariableValue val;
    index += val.readName(buf,index);
    index ++; // ignore 03 ??
    index ++; // jump to counter
    numberOfIds = (unsigned char) buf[index];
    index ++; // Ignore 00 ??
    index ++;
    sids = new std::vector<char *>;
     for(int i = 0 ; i < numberOfIds ; i++) {
      index += val.readValue(buf,index,16);
      char *sid = new char[16];
      memcpy(sid,val.getValue(),16);
      sids->push_back(sid);
    }

    index += val.readName(buf,index);
    XDEBUG(( "valname %s\n", val.getName().c_str() ));
    index ++; // Ignore 04 ??
    msgs = new std::vector<std::string>;
    index = readStrings(msgs,buf,index);

  }


int RecvStatusMessagePacket::readStrings(std::vector<std::string> *strings, char *buf, int index) {
    VariableValue friends;
    index += friends.readValue(buf,index);
    index ++; // Ignore 00
    int numberOfStrings = friends.getValueAsLong();
    XDEBUG(( "name: %s numberOfStrings: %d\n", friends.getName().c_str(), numberOfStrings ));
    for(int i = 0 ; i < numberOfStrings ; i++) {
      int length = (unsigned char)buf[index++];
      index++;
      index += friends.readValue(buf,index,length);
      std::string stringvalue = std::string(friends.getValue(),length);
      strings->push_back(stringvalue);
      XDEBUG(( "String length: %2d : %s\n", length, stringvalue.c_str() ));
    }
    return index;
  }
};
