
#ifndef __XFIREUTILS_H
#define __XFIREUTILS_H

namespace xfirelib {

class XFireUtils {
 public:
  XFireUtils();
  /**
   *Adds the name of a attribute with its length to the packet
   */
  static int addAttributName(char *packet,int packet_length, char *att);
};

};

#endif
