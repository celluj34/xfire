

#include "buddylistonlinepacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include <vector>
#include "xdebug.h"
#include <iostream>

namespace xfirelib {
  using namespace std;

  void BuddyListOnlinePacket::parseContent(char *buf, int length, int numberOfAtts) {
    XINFO(( "Got List of buddys that are online\n" ));
    int index = 0;
    // friends
    VariableValue userid;
    userids = new vector<long>;

    index += userid.readName(buf,index);
    index ++; // Ignore 04
    index ++; // Ignore 02

    int numberOfIds = buf[index];
    index++;
    index++;//ignore 00
    for(int i = 0 ; i < numberOfIds ; i++) {
      index += userid.readValue(buf,index,4);
      userids->push_back(userid.getValueAsLong());
      XINFO(( "UserID: %d\n", userid.getValueAsLong() ));
    }

    VariableValue sid;
    sids = new vector<char *>;
    index += sid.readName(buf,index);
    index ++; // Ignore 04
    index ++; // Ignore 03

    int numberOfSids = buf[index];
    index++;
    index++;//ignore 00
    for(int i = 0 ; i < numberOfIds ; i++) {
      index += userid.readValue(buf,index,16);
      char *sid = new char[16];
      memcpy(sid,userid.getValue(),16);
      sids->push_back(sid);
      //for(int loop = 0; loop < userid.getValueLength();loop++){
      //      XINFO(( "SID: %d\n", userid.getValue()[loop] ));
      //}
    }

  }

};
