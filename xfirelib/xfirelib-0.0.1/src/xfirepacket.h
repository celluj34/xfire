#ifndef __XFIREPACKET_H
#define __XFIREPACKET_H

#include "packetreader.h"
#include "xfirepacketcontent.h"

namespace xfirelib {
  struct PacketReader;
  struct XFirePacketContent;

  class XFirePacket {
  public:
    XFirePacket(PacketReader *reader);
    XFirePacket(XFirePacketContent *content);
    ~XFirePacket();

    int getSendBuffer(void *buf);
    void recvPacket(Socket *socket);
    void sendPacket(Socket *socket);

    XFirePacketContent *getContent() { return content; }
  private:
    PacketReader *reader;

    XFirePacketContent *content;
  };
};


#endif
