

#include "loginfailedpacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include <iostream>

namespace xfirelib {
  using namespace std;

  void LoginFailedPacket::parseContent(char *buf, int length, int numberOfAtts) {
    cout << "Login failed, incorrect username or password" << endl;
  }

};
