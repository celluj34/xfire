#ifndef __SENDACCEPTINVITATION_H
#define __SENDACCEPTINVITATION_H

#include "xfiresendpacketcontent.h"
#include <string>


#define XFIRE_ACCEPT_INVITATION_PACKET 07

namespace xfirelib {
  class SendAcceptInvitationPacket : public XFireSendPacketContent {
  public:

    int getPacketId() { return XFIRE_ACCEPT_INVITATION_PACKET; }

    int getPacketContent(char *buf);
    int getPacketAttributeCount() { return 3; }
    int getPacketSize() { return 100; }

    std::string name;
  private:
    
  };

};


#endif

