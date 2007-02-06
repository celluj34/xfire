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


#include "recvnicknamechangepacket.h"
#include "variablevalue.h"
#include "xdebug.h"

#include <string>

namespace xfirelib {

  RecvNicknameChangePacket::RecvNicknameChangePacket() {
    
  }

  void RecvNicknameChangePacket::parseContent(char *buf, int length, int numberOfAtts) {
    int index = 2;
    
    VariableValue val;
    index += val.readValue(buf,index,4);
    this->userId = val.getValueAsLong();
    XDEBUG(( "UserID: %ld\n", userId ));
    index++; index++;
    index += val.readValue(buf,index,2);
    long nicknameLength = val.getValueAsLong();
    XDEBUG(( "Nickname Length: %ld\n", nicknameLength ));
    val.readValue(buf,index,nicknameLength);
    this->nickname = std::string(val.getValue(),nicknameLength);
    XDEBUG(( "New nickname: %s\n", nickname.c_str() ));
  }

};
