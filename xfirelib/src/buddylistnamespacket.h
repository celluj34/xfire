#ifndef __BUDDYLISTNAMESPACKET_H
#define __BUDDYLISTNAMESPACKET_H

#define XFIRE_BUDDYS_NAMES_ID 131

#include <vector>
#include <string>

#include "xfirepacketcontent.h"
#include "variablevalue.h"


namespace xfirelib {
  using namespace std;

  class BuddyListNamesPacket : public XFirePacketContent {
  public:
    BuddyListNamesPacket();

    XFirePacketContent* newPacket() { return new BuddyListNamesPacket(); }

    int getPacketId() { return XFIRE_BUDDYS_NAMES_ID; }
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; };
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);

    //private:
    int readStrings(vector<string> *strings, char *buf, int index);

    vector<string> *usernames;
    vector<string> *nicks;
    vector<long> *userids;
  };
};


#endif
