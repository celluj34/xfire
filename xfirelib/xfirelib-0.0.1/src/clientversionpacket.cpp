#include "clientversionpacket.h"
#include "xfireutils.h"
#include <string.h>

namespace xfirelib {

  int ClientVersionPacket::getPacketContent(char *packet) {
	int index = 0;
	
	index = XFireUtils::addAttributName(packet,index, "version");/*add xfire version*/
	packet[index] = 0x02;
	packet[index+1] = (char) version;//protocol version
	packet[index+2] = 0x00;
	packet[index+3] = 0x00;
	packet[index+4] = 0x00;
	index += 5;
	length = index;
	return index;
  }

  int ClientVersionPacket::getPacketAttributeCount() {
    return 1;
  }

  void ClientVersionPacket::setProtocolVersion(int newVersion){
    version = newVersion;
  }

}
