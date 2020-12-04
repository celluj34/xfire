/*
 * File    : XfireConnection.java
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
package net.sphene.xfirelib;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.PacketListener;
import net.sphene.xfirelib.packets.PacketReader;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.buddylist.BuddyList;
import net.sphene.xfirelib.packets.content.MessageType;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.SendPacketContent;
import net.sphene.xfirelib.packets.content.recv.AuthPacket;
import net.sphene.xfirelib.packets.content.recv.MessageIncoming;
import net.sphene.xfirelib.packets.content.send.internal.ClientInformationPacket;
import net.sphene.xfirelib.packets.content.send.internal.ClientLoginPacket;
import net.sphene.xfirelib.packets.content.send.internal.ClientVersionPacket;
import net.sphene.xfirelib.packets.content.send.internal.KeepAlivePacket;
import net.sphene.xfirelib.packets.content.send.internal.MessageAckOutgoing;

/**
 * The main class for the xfire library which is responsible for connecting
 * to the xfire server and starting packet listeners, etc.
 * 
 * @author kahless
 */
public class XfireConnection implements PacketListener {
	public static final String XFIRE_HOST = "cs.xfire.com";
	public static final int XFIRE_PORT = 25999;
	public static final int XFIRE_VERSION = 100;
	
	
	private static final Logger logger = Logger.getLogger(XfireConnection.class.getName());
	
	protected String username;
	protected String password;
	private String xfireHost;
	private int xfirePort;
	
	
	private Socket socket;
	private PacketReader packetReader;
	
	private List<ConnectionListener> listenerList = new ArrayList<ConnectionListener>();
	
	private List<PacketListener> packetListenerList = new ArrayList<PacketListener>();
	private Thread sendKeepAlive;
	private BuddyList buddyList;
	
	
	/**
	 * Creates a new XfireConnection object for the default xfire host and port.
	 * It does not automatically connect, use {@link #connect(String, String)} afterwards. 
	 * 
	 * @param username
	 * @param password
	 */
	public XfireConnection() {
		this( XFIRE_HOST, XFIRE_PORT);
	}


	public XfireConnection(String xfireHost, int xfirePort) {
		this.xfireHost = xfireHost;
		this.xfirePort = xfirePort;
		this.addPacketListener(this);
		buddyList = new BuddyList(this);
	}
	
	/**
	 * connects and logins to the xfire server using the given username and password.
	 * @param username
	 * @param password
	 */
	public void connect(String username, String password) throws IOException {
		this.username = username;
		this.password = password;
		
		socket = new Socket(xfireHost, xfirePort);
		packetReader = new PacketReader(this, socket.getInputStream());
		
		
		packetReader.start();
		
		OutputStream outputStream = socket.getOutputStream();
		outputStream.write("UA01".getBytes());
		logger.info("Sent UA01");
		
		fireGotConnectedEvent();
		
		send(new ClientInformationPacket());
		send(new ClientVersionPacket());
		
		if (sendKeepAlive == null) {
			sendKeepAlive = new Thread() {
				@Override
				public void run() {
					try {
						Thread.sleep(3 * 60 * 1000); // Sleep for 3 minutes
					} catch (InterruptedException e) {
						e.printStackTrace();
						return;
					}
					send(new KeepAlivePacket());
				}
			};
			sendKeepAlive.start();
		}
		
		logger.info("Sent ClientInformationPacket / ClientVersionPacket");
	}


	public void receivedPacket(XfireRecvPacket packet) {
		RecvPacketContent content = packet.getPacketContent();
		if(content instanceof AuthPacket) {
			logger.fine("Got auth packet, sending login.");
			AuthPacket authPacket = (AuthPacket) content;
			ClientLoginPacket loginPacket = new ClientLoginPacket(username, password, authPacket.getSalt());
			send(loginPacket);
		} else if(content instanceof MessageIncoming) {
			MessageIncoming msg = (MessageIncoming) content;
			MessageType msgtype = msg.getMessageType();
			if(msgtype != null && msgtype.requiresAck()) {
				logger.fine("Received message, sending ack..");
				send(new MessageAckOutgoing(msg));
			}
		}
	}


	public void send(SendPacketContent packetContent) {
		XfireSendPacket packet = new XfireSendPacket(packetContent);
		try {
			fireSendingPacketEvent(packet);
			packet.sendPacket(socket.getOutputStream());
			fireSentPacketEvent(packet);
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error while sending packet {" + packetContent.getClass().getName() + "}", e);
		}
	}


	public PacketReader getPacketReader() {
		return packetReader;
	}
	
	
	
	private void fireGotConnectedEvent() {
		for(ConnectionListener listener : listenerList) {
			listener.gotConnected(this);
		}
	}
	private void fireSendingPacketEvent(XfireSendPacket packet) {
		for(PacketListener listener : packetListenerList) {
			listener.sendingPacket(packet);
		}
	}
	private void fireSentPacketEvent(XfireSendPacket packet) {
		for(PacketListener listener : packetListenerList) {
			listener.sentPacket(packet);
		}
	}
	public void addConnectionListener(ConnectionListener listener) {
		listenerList.add(listener);
	}
	public void removeConnectionListener(ConnectionListener listener) {
		listenerList.remove(listener);
	}
	public void addPacketListener(PacketListener listener) {
		this.packetListenerList.add(listener);
	}
	public void removePacketListener(PacketListener listener) {
		this.packetListenerList.remove(listener);
	}


	public List<PacketListener> getPacketListenerList() {
		return packetListenerList;
	}


	public void sendingPacket(XfireSendPacket packet) {
		// TODO Auto-generated method stub
		
	}


	public void sentPacket(XfireSendPacket packet) {
		// TODO Auto-generated method stub
		
	}

	
	public BuddyList getBuddyList() {
		return this.buddyList;
	}

}
