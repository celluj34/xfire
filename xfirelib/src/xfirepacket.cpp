/*
 *  xfirelib - C++ Library for the xfire protocol.
 *  Copyright (C) 2006 by
 *          Beat Wolf <asraniel@fryx.ch> / http://gfire.sf.net
 *          Herbert Poul <herbert.poul@gmail.com> / http://goim.us
 *    http://xfirelib.sphene.net
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


#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "xfirepacket.h"

#include "xdebug.h"

#include "socketexception.h"

namespace xfirelib {
  using namespace std;

  XFirePacket::XFirePacket(PacketReader *reader) {
    this->reader = reader;
    this->content = NULL;
  }
  XFirePacket::XFirePacket(XFirePacketContent *content) {
    this->content = content;
    this->reader = NULL;
  }

  XFirePacket::~XFirePacket() {
  }


  void XFirePacket::recvPacket(Socket *socket) {
    char buf[5];
    int r = socket->recv( (void *)buf, 5 );
    if(r != 5) {
      XERROR(("Returned less than 5 ?!: %d\n",r));
      throw SocketException("Connection Closed ?");
    }
    XDEBUG(( "Read %d bytes ...\n", r ));

    unsigned int len = ((unsigned char)buf[0]) + (((unsigned char)buf[1]) * 256);
    int packetid = buf[2];
    int zero = buf[3];
    int numberOfAtts = (unsigned char)buf[4];

    char contentbuf[len-5];
    XDEBUG(("calling socket->recv(contentbuf,%d)\n",len-5));
    int r2 = socket->recv( (void *)contentbuf, len-5 );

    XDEBUG(("packetid: %d numberOfAtts: %d length: %d Got: %d\n", (unsigned char)packetid,numberOfAtts,len-5,r2));
    XFirePacketContent *contentClass = reader->getPacketContentClass( (unsigned char)packetid );

    if(contentClass == NULL) {
      XERROR(("NO SUCH CONTENT PACKET (%d) :(\n", (unsigned char)packetid));
      return;
    }
    XDEBUG(("Creating ContentClass Instance\n"));
    content = contentClass->newPacket();
    content->parseContent(contentbuf, r2, numberOfAtts);
  }

  void XFirePacket::sendPacket(Socket *socket) {
    int size = content->getPacketSize();
    char *buf = (char*)malloc(size * sizeof(char));
    XDEBUG(("Allocated %d characters\n", size));
    int rsize = content->getPacketContent( buf );
    int realsize = rsize + 5;
    char *sendbuf = (char*)malloc(realsize * sizeof(char));
    memcpy( sendbuf + 5, buf, rsize );
    sendbuf[0] = realsize % 256;
    sendbuf[1] = (int)realsize / 256;
    sendbuf[2] = content->getPacketId();
    sendbuf[3] = 0;
    sendbuf[4] = content->getPacketAttributeCount();

    socket->send( sendbuf, realsize );
    free(buf); free(sendbuf);
  }

};
