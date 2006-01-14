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

#include "clientinformationpacket.h"
#include "xfireutils.h"
#include <string.h>
#include <iostream>

namespace xfirelib {
  using namespace std;

  int ClientInformationPacket::getPacketContent(char *packet) {
	int index = 0;
	int skins = 2;
	int i;

	index = XFireUtils::addAttributName(packet,index, "skin");/*add skin*/
	packet[index] = 0x04;
	packet[index+1] = 0x01;
	packet[index+2] = (char)skins;
	packet[index+3] = 0x00;
	packet[index+4] = strlen("Standard");
	packet[index+5] = 0x00;
	index += 6;
	
	memcpy(packet+index,"Standard",strlen("Standard"));/*add first skin name*/
	index += strlen("Standard");
	
	packet[index] = strlen("XFire");
	packet[index+1] = 0x00;
	index += 2;
	
	memcpy(packet+index,"XFire",strlen("XFire"));/*add second skin name*/
	index += strlen("XFire");	
	
	index = XFireUtils::addAttributName(packet,index, "version");/*add version of skins*/
	packet[index] = 0x04;
	packet[index+1] = 0x02;
	packet[index+2] = (char)skins;
	packet[index+1] = 0x00;
	index += 4;
	
	for(i = 0;i < skins;i++){/*(forEachSkin){ 01 00 00 00 }*/
		packet[index] = 0x01;
		packet[index+1] = 0x00;
		packet[index+2] = 0x00;
		packet[index+3] = 0x00;
		index += 4;
	}
	length = index;
	return index;
  }

  int ClientInformationPacket::getPacketAttributeCount() {
    return 2;
  }

}
