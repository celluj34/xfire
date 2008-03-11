/*
 *  xfiregateway - Jabber Gateway for XFire.
 *  Copyright (C) 2006 by
 *          Herbert Poul <herbert.poul@gmail.com> / http://goim.us
 *    http://xfirelib.sphene.net/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

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

    void handleMessage(Stanza *stanza, MessageSession *msg = 0);

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
