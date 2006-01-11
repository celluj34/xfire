#include "socket.h"

#ifndef __PACKETREADER_H
#define __PACKETREADER_H

#include <vector>
#include "xfirepacketcontent.h"
#include "packetlistener.h"

namespace xfirelib {
  struct PacketListener;
  struct XFirePacket;

class PacketReader {
 public:
  PacketReader(Socket *socket);
  ~PacketReader();

  void setSocket(Socket *socket);
  void startListening();
  XFirePacketContent *getPacketContentClass(int packetId);
  void addPacketListener( PacketListener *listener );
  void run();
 private:
  void initPackets();
  void fireListeners( XFirePacket *packet );

  Socket *socket;
  std::vector <XFirePacketContent *> *packets;
  std::vector <PacketListener *> *packetListeners;
};

};

#endif
