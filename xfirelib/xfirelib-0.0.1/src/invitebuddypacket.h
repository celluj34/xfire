#ifndef __INVITEBUDDYPACKET_H
#define __INVITEBUDDYPACKET_H

#include "xfirepacketcontent.h"
#include <vector>
#include <string>

#define XFIRE_PACKET_INVITE_BUDDY 6

namespace xfirelib {
  using namespace std;

  /**
   * Packet Used to Invite Buddy into Contact List
   * (Received is InviteRequestPacket)
   */
  class InviteBuddyPacket : public XFirePacketContent {
  public:
    InviteBuddyPacket();
    ~InviteBuddyPacket();

    XFirePacketContent* newPacket() { return new InviteBuddyPacket(); }


    int getPacketId() { return XFIRE_PACKET_INVITE_BUDDY; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 2; };
    int getPacketSize() { return 100; };
    void parseContent(char *buf, int length, int numberOfAtts);

    void addInviteName(string name, string msg);

    vector<string> *names;
    vector<string> *msgs;
  private:
  };

};



#endif
