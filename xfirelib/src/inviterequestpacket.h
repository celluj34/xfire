#ifndef __INVITEREQUESTPACKET_H
#define __INVITEREQUESTPACKET_H

#include <string>
#include "xfirepacketcontent.h"

#define XFIRE_PACKET_INVITE_REQUEST_PACKET 138

namespace xfirelib {
  using namespace std;

  /**
   * Received when someone invites you to his buddylist
   * (For sending invitations see InviteBuddyPacket)
   */
  class InviteRequestPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new InviteRequestPacket(); }


    int getPacketId() { return XFIRE_PACKET_INVITE_REQUEST_PACKET; }
    int getPacketContent(char *buf) { }
    int getPacketAttributeCount() { return 3; };
    int getPacketSize() { return 100; };
    void parseContent(char *buf, int length, int numberOfAtts);

    string name;
    string nick;
    string msg;

  private:
  };

};


#endif

