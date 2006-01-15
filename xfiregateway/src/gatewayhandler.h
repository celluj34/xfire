#ifndef __GATEWAYHANDLER_H
#define __GATEWAYHANDLER_H

#include "xfiregateway.h"
#include <gloox/iqhandler.h>
#include <gloox/stanza.h>
#include <gloox/component.h>
#include <gloox/presencehandler.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/subscriptionhandler.h>

namespace xfiregateway {
  using namespace gloox;

  class GatewayHandler : 
    public IqHandler, public PresenceHandler, public MessageHandler,
    public ConnectionListener, public SubscriptionHandler {
  public:
    GatewayHandler(XFireGateway *gateway);

    bool handleIq(Stanza *stanza);
    bool handleIqID(Stanza *stanza, int context);

    void handlePresence(Stanza *stanza);
    void handleSubscription(Stanza *stanza);

    void handleMessage(Stanza *stanza);

    void onConnect();
    void onDisconnect(ConnectionError error);
    void onResourceBindError(ResourceBindError error);
    void onSessionCreateError(SessionCreateError error);
    bool onTLSConnect(const CertInfo &info);

  private:
    XFireGateway *gateway;
    Component *comp;
  };

};



#endif
