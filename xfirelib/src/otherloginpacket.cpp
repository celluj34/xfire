

#include "otherloginpacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include "xdebug.h"

namespace xfirelib {
  using namespace std;

  void OtherLoginPacket::parseContent(char *buf, int length, int numberOfAtts) {
    XINFO(("Someone login in with the same account that we have\n"));
  }

};
