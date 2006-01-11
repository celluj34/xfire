
#include <vector>
#include <string>

#include "buddylistnamespacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {
  using namespace std;

  BuddyListNamesPacket::BuddyListNamesPacket() {
    usernames = 0;
    nicks = 0;
    userids = 0;
  }

  void BuddyListNamesPacket::parseContent(char *buf, int length, int numberOfAtts) {
    int index = 0;
    // friends
    VariableValue friends;
    index += friends.readName(buf,index);
    index ++; // Ignore 04

    usernames = new vector<string>;
    index = readStrings(usernames,buf,index);

    index += friends.readName(buf,index);
    index ++; // Ignore 04

    nicks = new vector<string>;
    index = readStrings(nicks,buf,index);

    index += friends.readName(buf,index);
    index ++; // Ignore 04
    index += friends.readValue(buf,index);
    userids = new vector<long>;
    int numberOfIds = friends.getValueAsLong();
    for(int i = 0 ; i < numberOfIds ; i++) {
      index += friends.readValue(buf,index,4);
      userids->push_back(friends.getValueAsLong());
      XDEBUG(( "UserID: %d\n", friends.getValueAsLong() ));
    }
  }

  int BuddyListNamesPacket::readStrings(vector<string> *strings, char *buf, int index) {
    VariableValue friends;
    index += friends.readValue(buf,index);
    index ++; // Ignore 00
    int numberOfStrings = friends.getValueAsLong();
    XDEBUG(( "name: %s numberOfStrings: %d\n", friends.getName().c_str(), numberOfStrings ));
    for(int i = 0 ; i < numberOfStrings ; i++) {
      int length = (unsigned char)buf[index++];
      index++;
      index += friends.readValue(buf,index,length);
      string stringvalue = string(friends.getValue(),length);
      strings->push_back(stringvalue);
      XDEBUG(( "String length: %2d : %s\n", length, stringvalue.c_str() ));
    }
    return index;
  }

};
