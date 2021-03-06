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

#ifndef __PARSEGAMEXML_H
#define __PARSEGAMEXML_H

#include "parser.h"
#include <gloox/stanza.h>

#include <xfirelib/xfiregame.h>
#include <xfirelib/xfiregameresolver.h>
#include <xfirelib/buddylistgamespacket.h>
#include <xfirelib/xdebug.h>

#include <map>

namespace xfiregateway {
  using namespace gloox;

  struct GOIMGameInfo;
  class ParseGameXML : public Parser, public xfirelib::XFireGameResolver {
  public:
    ParseGameXML();


    void streamEvent( Stanza *stanza );

    virtual xfirelib::XFireGame *resolveGame(int gameid, int iterator, xfirelib::BuddyListGamesPacket *packet);
    GOIMGameInfo *getGameInfoByGOIMId(std::string goimid);

  protected:
    std::map<int,GOIMGameInfo *> games;

    struct duo
    {
      duo( std::string f, std::string s ) : first( f), second( s ) {};
      std::string first;
      std::string second;
    };
    typedef std::list<duo> Duo;

    const std::string replace( const std::string& what, const Duo& duo ) const;
    const std::string relax( const std::string& what ) const;
  };

  class GOIMGameInfo {
  public:
    GOIMGameInfo( int xfireid, std::string goimid, std::string name, bool changestatus) {
      this->xfireid = xfireid;
      this->goimid = goimid;
      this->name = name;
      this->changestatus = changestatus;
    }
    ~GOIMGameInfo() {
      XERROR(( "Destructor of GOIMGameInfo .. this shouldn't happen until xfiregateway is exiting.\n" ));
    }
    int xfireid;
    bool changestatus;
    std::string goimid;
    std::string name;
  };

  class GOIMGame : public xfirelib::XFireGame {
  public:
    GOIMGame(GOIMGameInfo *info) {
      this->info = info;
    }
    // TODO !!!!!!!!!!!!! this->packet = packet should NOT be used !!
    // we need to create a copy of packet, because it will be deleted as soon
    // as all listeners are notified !!!
    void init(xfirelib::BuddyListGamesPacket *packet, int iterator) {
      this->packet = packet;
      memcpy(ip,packet->ips->at(iterator),4);
      port = packet->ports->at(iterator);
    }

    std::string getTarget() {
      XDEBUG(( "getting target...\n" ));
      char str[30];
      sprintf( str, "%d.%d.%d.%d:%ld",
	       (unsigned char)ip[3],
	       (unsigned char)ip[2],
	       (unsigned char)ip[1],
	       (unsigned char)ip[0], port );
      //sprintf( str, "%d.%d.%d.%d:%ld",
      //(unsigned char)ip[0],
      //(unsigned char)ip[1],
      //(unsigned char)ip[2],
      //(unsigned char)ip[3], port );
      return std::string( str );
    }

    int getGameId() { return info->xfireid; }
    std::string getGameName() { return info->name; }

    GOIMGameInfo *info;
    xfirelib::BuddyListGamesPacket *packet;
    char ip[4];
    long port;
  };

};



#endif
