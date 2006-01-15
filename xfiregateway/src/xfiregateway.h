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

#ifndef __XFIREGATEWAY_H
#define __XFIREGATEWAY_H

#include <string>
#include <gloox/component.h>
#include <vector>
#include "user.h"
#include "parsegamexml.h"

namespace xfiregateway {
  using namespace std;

  struct User;

  class XFireGateway {
  public:
    XFireGateway(std::map<std::string,std::string> &args);
    ~XFireGateway();
    void run();
    std::string getFQDN();

    gloox::Component *getComponent() { return comp; }

    void writeUserFile();
    void readUserFile();

    User *getUserByJID(std::string barejid);
    void addUser(User *user);
    void removeUser(User *user);
    void quit();
    std::vector<User*> *getUsers() { return users; }

    xfiregateway::ParseGameXML *gameXML;
  private:
    std::map<std::string,std::string> args;
    std::vector<std::string> explodeString(std::string s,
					   std::string e);

    gloox::Component *comp;

    std::vector<User*> *users;
  };

};


#endif

