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


#include "parsegamexml.h"
#include <xfirelib/xdebug.h>

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <gloox/tag.h>

namespace xfiregateway {
  using namespace gloox;
  
  ParseGameXML::ParseGameXML() : Parser( ) {
    XDEBUG(( "I'm in ParseGameXML() constructor.\n" ));
    //feed( "<stream:stream><?xml version=\"1.0\" encoding=\"UTF-8\"?><xml><abc a='b'><hehe /></abc><hehe><def /></hehe>blahblah</xml></stream:stream>" );

    struct stat st;
    if(stat("xfiregateway_users.cfg", &st) != 0) {
      XINFO(( "No user file yet ?\n" ));
      return;
    }
    std::fstream f("plugin.xml", std::ios_base::in);
    while(!f.eof()) {
      std::string str;
      std::getline(f,str);
      feed( str );
    }
    /*
    feed( "<?xml version=\"1.0\" encoding=\"UTF-8\"?><xml><abc a='b'><hehe /></abc><hehe><def /></hehe>blahblah" );
    feed( "</xml>" );
    */
  }

  void ParseGameXML::streamEvent( Stanza *stanza ) {
    XDEBUG(( "streamEvent .. START\n" ));
    if(!stanza) {
      XDEBUG(( "no stanza ?!\n" ));
      return;
    }
    if(stanza->name() == "") {
      XDEBUG(( "no name ?!\n" ));
    }
    XDEBUG(( "streamEvent(): %s\n", stanza->name().c_str() ));

    Tag *tag = stanza->findChildWithAttrib( "point", "net.sphene.goim.rcp.games" );
    if(!tag) {
      XERROR(( "No extension point ?!\n" ));
      return;
    }
    gloox::Tag::TagList list = tag->children();
    for( gloox::Tag::TagList::iterator it = list.begin() ;
	 it != list.end() ; it++ ) {
      Tag* xfireid = (*it)->findChildWithAttrib( "name", "xfireid" );
      if(xfireid) {
	int id = atoi( xfireid->findAttribute("value").c_str() );
	GOIMGameInfo *game = new GOIMGameInfo(id,
					      (*it)->findAttribute( "id" ),
					      this->relax((*it)->findAttribute( "name" )));
	games[game->xfireid] = game;
	XINFO(( "XFireID of %s is %d\n", (*it)->findAttribute( "id" ).c_str(), id ));
      }
    }

    XDEBUG(( "streamEvent .. END\n------------\n" ));
  }
  const std::string ParseGameXML::replace( const std::string& what, const Duo& duo ) const
  {
    std::string esc = what;
    Duo::const_iterator it = duo.begin();
    for( ; it != duo.end(); ++it )
      {
	size_t lookHere = 0;
	size_t foundHere;
	while( ( foundHere = esc.find( (*it).first, lookHere ) ) != std::string::npos )
	  {
	    esc.replace( foundHere, (*it).first.size(), (*it).second );
	    lookHere = foundHere + (*it).second.size();
	  }
      }
    return esc;
  }
  const std::string ParseGameXML::relax( const std::string& what ) const
  {
    Duo d;
    d.push_back( duo( "&lt;", "<" ) );
    d.push_back( duo( "&gt;", ">" ) );
    d.push_back( duo( "&apos;", "'" ) );
    d.push_back( duo( "&quot;", "\"" ) );
    d.push_back( duo( "&amp;", "&" ) );

    return replace( what, d );
  }


  xfirelib::XFireGame *ParseGameXML::resolveGame(int gameid, int iterator, xfirelib::BuddyListGamesPacket *packet) {
    XDEBUG(( "resolveGame( ... )\n" ));
    GOIMGameInfo *info = games[gameid];
    if(!info) return NULL;
    GOIMGame *game = new GOIMGame(info);
    game->init(packet,iterator);
    XDEBUG(( "END resolveGame( ... )\n" ));
    return game;
  }

};