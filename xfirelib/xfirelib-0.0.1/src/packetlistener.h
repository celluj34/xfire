#ifndef __PACKETLISTENER_H
#define __PACKETLISTENER_H

#include "xfirepacket.h"


namespace xfirelib {
  struct XFirePacket;

  class PacketListener {
  public:
    virtual void receivedPacket(XFirePacket *packet) = 0;
  };

};


#endif
