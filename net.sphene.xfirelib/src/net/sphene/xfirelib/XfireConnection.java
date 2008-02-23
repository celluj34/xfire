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
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.PacketListener;
import net.sphene.xfirelib.packets.PacketReader;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.SendPacketContent;
import net.sphene.xfirelib.packets.content.recv.AuthPacket;
import net.sphene.xfirelib.packets.content.send.internal.ClientInformationPacket;
import net.sphene.xfirelib.packets.content.send.internal.ClientLoginPacket;
import net.sphene.xfirelib.packets.content.send.internal.ClientVersionPacket;

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
		
		packetReader.addPacketListener(this);
		
		packetReader.start();
		
		OutputStream outputStream = socket.getOutputStream();
		outputStream.write("UA01".getBytes());
		logger.info("Sent UA01");
		
		send(new ClientInformationPacket());
		send(new ClientVersionPacket());
		
		logger.info("Sent ClientInformationPacket / ClientVersionPacket");
	}


	public void receivedPacket(XfireRecvPacket packet) {
		RecvPacketContent content = packet.getPacketContent();
		if(content instanceof AuthPacket) {
			logger.fine("Got auth packet, sending login.");
			AuthPacket authPacket = (AuthPacket) content;
			ClientLoginPacket loginPacket = new ClientLoginPacket(username, password, authPacket.getSalt());
			send(loginPacket);
		}
	}


	public void send(SendPacketContent packetContent) {
		XfireSendPacket packet = new XfireSendPacket(packetContent);
		try {
			packet.sendPacket(socket.getOutputStream());
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error while sending packet {" + packetContent.getClass().getName() + "}", e);
		}
	}
}
