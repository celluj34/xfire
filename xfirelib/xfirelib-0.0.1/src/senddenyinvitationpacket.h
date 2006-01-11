#ifndef __SENDDENYINVITATIONPACKET_H
#define __SENDDENYINVITATIONPACKET_H

#include "sendacceptinvitationpacket.h"

#define XFIRE_DENY_INVITATION_PACKET 8

namespace xfirelib {

  class SendDenyInvitationPacket : public SendAcceptInvitationPacket {
  public:
    int getPacketId() { return XFIRE_DENY_INVITATION_PACKET; }

  private:
  };

};


#endif
