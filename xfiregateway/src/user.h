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

#ifndef __USERS_H
#define __USERS_H

#include <string>
#include <vector>
#include <map>
#include <gloox/stanza.h>
#include <gloox/gloox.h>
#include <gloox/component.h>

#include <xfirelib/client.h>
#include <xfirelib/packetlistener.h>
#include <xfirelib/xfirepacket.h>

#include "monitoredobj.h"
#include "xfiregateway.h"
#include "parsegamexml.h"

namespace xfiregateway {
  using namespace gloox;

  struct XFireGateway;
  struct MonitoredObj;

  class UserPresence : public MonitoredObj {
  public:
    UserPresence();
    UserPresence(int status, std::string statusmsg, std::string resource, int priority);

    ~UserPresence();

    int status;
    std::string statusmsg;
    std::string resource;
    int priority;
    std::vector<GOIMGame *> games;
  };

  class User : public MonitoredObj, public xfirelib::PacketListener {
  public:
    User(XFireGateway *gateway);
    ~User();
    void receivedPresence(Stanza *stanza);
    void receivedMessage(Stanza *stanza);
    UserPresence *getPresence();

    void receivedPacket(xfirelib::XFirePacket *packet);
    bool isXFireConnected() { return client != NULL; }
    bool isOnline() { return presences.size() > 0; }
    void mirrorPresence();
    void sendInitialPresences();
    bool isInRoster(std::string username);
    void sendPresenceTagForBuddy(xfirelib::BuddyListEntry *buddy);
    void preRemove();

    std::string jid;
    std::string name;
    std::string password;
    std::vector<std::string> inroster;
    typedef std::map<std::string, UserPresence *> UserPresenceMap;
    UserPresenceMap presences;
    xfirelib::Client *client;

  protected:
    void createPacketExtension( xfirelib::XFireGame *game, Tag *parent, std::string &awaymsg );
    void presenceChanged();

    XFireGateway *gateway;
    gloox::Component *comp;
  };

};


#endif
