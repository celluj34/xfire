/*
 * File    : ClientInformationPacket.java
 * Created : 23.02.2008
 * By      : kahless
 *
 * java xfirelib - Java library for the xfire protocol
 * Copyright (C) 2008 Herbert Poul
 *              (JabberId: kahless@sphene.net / Email: herbert.poul@gmail.com)
 * http://xfirelib.sphene.net
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */
package net.sphene.xfirelib.packets.content.send.internal;

import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.content.SendPacketContent;

public class ClientInformationPacket extends SendPacketContent {

	@Override
	public int getPacketId() {
		return 18;
	}

	@Override
	public void fillPacketContent(XfireSendPacket packet) {
		int skins = 2;
		
		// Parameter: Skin
		packet.addAttributeName("skin");
		packet.write(0x04);
		packet.write(0x01);
		packet.write(skins);
		packet.write(0x00);
		
		// Add first skin
		byte[] buf = "Standard".getBytes();
		packet.write(buf.length);
		packet.write(0x00);
		packet.write(buf);
		
		// Add second skin
		buf = "XFire".getBytes();
		packet.write(buf.length);
		packet.write(0x00);
		packet.write(buf);
		
		// Parameter: Version
		packet.addAttributeName("version");
		packet.write(0x04);
		packet.write(0x02);
		packet.write(skins);
		packet.write(0x00);
		
		for(int i = 0 ; i < skins ; i++) {
			packet.write(0x01);
			packet.write(0x00);
			packet.write(0x00);
			packet.write(0x00);
		}
	}

}
