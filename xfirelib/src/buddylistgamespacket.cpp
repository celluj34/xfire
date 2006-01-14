
#include <vector>
#include <string>

#include "buddylistgamespacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {
  using namespace std;

  BuddyListGamesPacket::BuddyListGamesPacket() {
  }

  void BuddyListGamesPacket::parseContent(char *buf, int length, int numberOfAtts) {
    int index = 0;
    int numberOfSids = 0;
    VariableValue val;
    
    gameids = new vector<long>;
    index += val.readName(buf,index);
    index ++; // Ignore 04
    index ++; // Ignore 03
    numberOfSids = (unsigned char) buf[index];
    index ++; // Ignore 00
    index ++;
    sids = new vector<char *>;
    for(int i = 0 ; i < numberOfSids ; i++) {
      index += val.readValue(buf,index,16);
      char *sid = new char[16];
      memcpy(sid,val.getValue(),16);
      sids->push_back(sid);
    }

    index += val.readName(buf,index);
    index ++; // Ignore 04
    index ++; // Ignore 03
    numberOfSids = (unsigned char) buf[index];
    index ++; // Ignore 00
    XINFO(("number valname %s \n",val.getName().c_str()));
    XINFO(("number gameids %d \n",numberOfSids));
    for(int i = 0 ; i < numberOfSids ; i++) {
      index += val.readValue(buf,index,4);
        long game = ((unsigned char)val.getValue()[0])+((unsigned char)val.getValue()[1])*256;
      gameids->push_back(game);
        XINFO(("someone plays %d \n",game));
    }
  }


};
