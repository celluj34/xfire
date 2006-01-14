
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

    gameids = new vector<long>;
    gameids2 = new vector<long>;
    for(int i = 0 ; i < numberOfSids ; i++) {
      index += val.readValue(buf,index,4);
        long game = ((unsigned char)val.getValue()[0])*256+((unsigned char)val.getValue()[1]);
        long game2 = ((unsigned char)val.getValue()[2])*256+((unsigned char)val.getValue()[3]);
        gameids->push_back(game);
        gameids2->push_back(game2);
        XINFO(("someone plays %d \n",game));
    }
  }


};
