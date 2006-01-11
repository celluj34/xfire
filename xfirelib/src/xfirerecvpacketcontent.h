
#ifndef __XFIRERECVPACKETCONTENT_H
#define __XFIRERECVPACKETCONTENT_H

#include "xfirepacketcontent.h"

namespace xfirelib {
  
  class XFireRecvPacketContent : public XFirePacketContent {
  public:
    int getPacketContent(char *buf) { return 0; }
    int getPacketAttributeCount() { return 0; }
    int getPacketSize() { return 0; }
  };

};


#endif

