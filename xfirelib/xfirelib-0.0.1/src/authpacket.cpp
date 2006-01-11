

#include "authpacket.h"
#include "xfireparse.h"
#include "variablevalue.h"

namespace xfirelib {
  using namespace std;

  void AuthPacket::parseContent(char *buf, int length, int numberOfAtts) {
	
    //XFireParse parse;
    salt = new VariableValue();
    //parse.readVariableValue(buf, 0, length, salt);
    salt->readVariableValue(buf, 0, length);
  }

};
