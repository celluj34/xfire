
#include "xdebug.h"
#include <string.h>
#include <iostream>
#include "xfireutils.h"


namespace xfirelib {

using namespace std;

  XFireUtils::XFireUtils() {
  }

  int XFireUtils::addAttributName(char *packet,int packet_length, char *att){
    XDEBUG(( "Adding %d chars at position %d\n",strlen(att),packet_length));
	packet[packet_length] = (char)strlen(att);//set att length
	memcpy(packet+packet_length+1,att,strlen(att)); //set attname
	return packet_length+1+strlen(att);
  }
};
