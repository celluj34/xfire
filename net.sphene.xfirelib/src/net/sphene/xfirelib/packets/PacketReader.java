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

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.omg.PortableInterceptor.SUCCESSFUL;

import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.recv.AuthPacket;
import net.sphene.xfirelib.packets.content.recv.GenericRecvPacket;
import net.sphene.xfirelib.packets.content.recv.IgnoreRecvPacket;
import net.sphene.xfirelib.packets.content.recv.LoginFailedPacket;
import net.sphene.xfirelib.packets.content.recv.LoginSuccessPacket;

public class PacketReader extends Thread {
	private static Logger logger = Logger.getLogger(PacketReader.class.getName());
	
	private List<PacketListener> listenerList = new ArrayList<PacketListener>();
	
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
		addPacketContent(new LoginFailedPacket());
		addPacketContent(new LoginSuccessPacket());
		
		ignorePacketIds(new int[] {
				141, // ignore preference packet ..
				151,
				152,
				155,
				157,
				177,
				163,
				144, // DID packet ?!
		}
		);
//		recvpacketcontent.put(141, new IgnoreRecvPacket()); // ignore preference packet ..
//		recvpacketcontent.put(151, new IgnoreRecvPacket());
//		recvpacketcontent.put(152, new IgnoreRecvPacket());
//		recvpacketcontent.put(155, new IgnoreRecvPacket());
//		recvpacketcontent.put(157, new IgnoreRecvPacket());
//		recvpacketcontent.put(177, new IgnoreRecvPacket());
//		recvpacketcontent.put(163, new IgnoreRecvPacket());
	}

	private void ignorePacketIds(int[] ids) {
		IgnoreRecvPacket ignore = new IgnoreRecvPacket();
		for(int id : ids) {
			recvpacketcontent.put(id, ignore);
		}
	}

	private void addPacketContent(RecvPacketContent packetContent) {
		recvpacketcontent.put(packetContent.getPacketId(), packetContent);
	}

	@Override
	public void run() {
		while(true) {
			XfireRecvPacket packet = new XfireRecvPacket(this);
			try {
				logger.info("waiting for packet to be received..");
				packet.recvPacket(inputStream);
			} catch (IOException e) {
				logger.log(Level.SEVERE, "error while receiving packet", e);
				break;
			}
			
			if(packet.getPacketContent() != null) {
				logger.info("Received a new packet of type: id {" +
						packet.getPacketContent().getPacketId() +
						"} / class: {" +
						packet.getPacketContent().getClass().getName() + "}");
				fireListeners(packet);
			} else {
				logger.warning("Received packet with unknown id {" + packet.getPacketId() + "}");
			}
		}
	}
	
	private void fireListeners(XfireRecvPacket packet) {
		for(PacketListener listener : listenerList) {
			listener.receivedPacket(packet);
		}
	}

	public void addPacketListener(PacketListener listener) {
		this.listenerList.add(listener);
	}
	public void removePacketListener(PacketListener listener) {
		this.listenerList.remove(listener);
	}

	public RecvPacketContent createPacketContentById(int packetId) {
		RecvPacketContent packetContent = recvpacketcontent.get(packetId);
		if (packetContent == null) {
			return new GenericRecvPacket();
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
