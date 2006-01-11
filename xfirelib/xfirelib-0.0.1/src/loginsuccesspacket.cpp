#include "xdebug.h"
#include "loginsuccesspacket.h"


namespace xfirelib {

  void LoginSuccessPacket::parseContent(char *buf, int length, int numberOfAtts) {
    int read = 0;
    for(int i = 0 ; i < numberOfAtts ; i++) {
      VariableValue *val = new VariableValue();
      read += val->readName(buf, read);
      XDEBUG(( "Read Variable Name: %s\n", val->getName().c_str() ));
      if(val->getName() == "userid") {
	read++; // ignore 02
	read += val->readValue(buf, read, 3);
	read++; // ignore 00
	XDEBUG(( "My userid: %u\n", val->getValueAsLong() ));
      } else if(val->getName() == "sid") {
	read++; // ignore 03
	read+=val->readValue(buf, read, 16);
	//XDEBUG(( "My SID: %u\n", val->getValue() ));
      } else if(val->getName() == "nick") {
	//read+=val->readValue(buf, read);
	//int lengthLength = (int)val->getValueAsLong();
	//XDEBUG(( "Nick Length: %d\n", lengthLength ));
	read++; // ignore 01
	read+=val->readValue(buf, read, -1, 1);
      } else if(val->getName() == "status") {
	read+=5; // ignore everything
      } else if(val->getName() == "dlset") {
	read+=3; // ignore everything
      } else {
	i = numberOfAtts; 
	// If we find something we don't know .. we stop parsing the 
	// packet.. who cares about the rest...
      }
    }
  }


};
