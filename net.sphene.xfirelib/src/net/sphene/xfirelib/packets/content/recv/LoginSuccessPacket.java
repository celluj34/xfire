/*
 * File    : LoginSuccessPacket.java
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
package net.sphene.xfirelib.packets.content.recv;

import net.sphene.xfirelib.packets.XfireAttribute;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.content.RecvPacketContent;

public class LoginSuccessPacket extends RecvPacketContent {

	@Override
	public void parseContent(XfireRecvPacket packet, int numberOfAtts) {
		for(int i = 0 ; i < numberOfAtts ; i++) {
			XfireAttribute attr = packet.readAttribute();
			System.out.println("attr {" + attr.getName() + "} / value {" + new String(attr.getValue()) + "}");
		}
	}

	@Override
	public int getPacketId() {
		return 130;
	}

}
