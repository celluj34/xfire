#ifndef __XFIRESENDPACKETCONTENT_H
#define __XFIRESENDPACKETCONTENT_H

#include "xfirepacketcontent.h"

namespace xfirelib {

  class XFireSendPacketContent : public XFirePacketContent {
  public:
    /**
     * This method is not needed for outgoing packets.. since this method is only
     * used for parsing .. so return null
     */
    XFirePacketContent *newPacket() { return 0; }
    void parseContent(char *buf, int length, int numberOfAtts) { }
  };

};


#endif
