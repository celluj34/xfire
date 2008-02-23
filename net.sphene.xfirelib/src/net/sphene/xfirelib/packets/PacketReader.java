/*
 * File    : PacketReader.java
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
package net.sphene.xfirelib.packets;

import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.recv.AuthPacket;

public class PacketReader extends Thread {
	private static Logger logger = Logger.getLogger(PacketReader.class.getName());
	
	private XfireConnection xfireConnection;
	private InputStream inputStream;
	
	private Map<Integer, RecvPacketContent> recvpacketcontent = new HashMap<Integer, RecvPacketContent>();

	public PacketReader(XfireConnection xfireConnection, InputStream inputStream) {
		this.xfireConnection = xfireConnection;
		this.inputStream = inputStream;
		
		init();
	}
	
	protected void init() {
		// all packet contents which can be received
		addPacketContent(new AuthPacket());
	}

	private void addPacketContent(AuthPacket packetContent) {
		recvpacketcontent.put(packetContent.getPacketId(), packetContent);
	}

	@Override
	public void run() {
		
	}

	public RecvPacketContent createPacketContentById(int packetId) {
		RecvPacketContent packetContent = recvpacketcontent.get(packetId);
		if (packetContent == null) {
			return null;
		}
		try {
			return packetContent.getClass().newInstance();
		} catch(Exception e) {
			logger.log(Level.SEVERE,
						"Error while creating new instance for packet content id {" +
						packetId + "} / {" + packetContent.getClass().getName() + "}");
		}
		return null;
	}
}
