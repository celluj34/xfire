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
import java.net.Socket;
import java.net.UnknownHostException;

import net.sphene.xfirelib.packets.PacketReader;

/**
 * The main class for the xfire library which is responsible for connecting
 * to the xfire server and starting packet listeners, etc.
 * 
 * @author kahless
 */
public class XfireConnection {
	public static final String XFIRE_HOST = "cs.xfire.com";
	public static final int XFIRE_PORT = 25999;
	
	
	
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
		packetReader.start();
	}
}
