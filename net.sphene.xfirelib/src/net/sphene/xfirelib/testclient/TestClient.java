/*
 * File    : TestClient.java
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
package net.sphene.xfirelib.testclient;

import java.awt.GraphicsEnvironment;
import java.io.IOException;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

import net.sphene.xfirelib.ConnectionListener;
import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.debug.PacketDebugger;
import net.sphene.xfirelib.packets.buddylist.BuddyList;

public class TestClient implements ConnectionListener {
	private static Logger logger = Logger.getLogger(TestClient.class.getName());
	
	private String username;
	private String password;
	private XfireConnection conn;


	public TestClient(String username, String password) {
		this.username = username;
		this.password = password;
	}

	public static void main(String[] args) {
		try {
			LogManager.getLogManager().readConfiguration(TestClient.class.getResourceAsStream("logging.properties"));
		} catch (IOException e) {
			logger.log(Level.SEVERE, "error while reading logging configuration", e);
		}
		
		if(args.length < 2) {
			System.err.println("Required Arguments: <xfire username> <xfire password>");
			System.exit(1);
		}
		String username = args[0];
		String password = args[1];
		
		new TestClient(username, password).run();
	}

	private void run() {
		conn = new XfireConnection();
		new SwingWindow(conn);
		try {new PacketDebugger(conn);}
		catch(Exception e) {
			e.printStackTrace();
		}
		conn.addConnectionListener(this);
		try {
			conn.connect(username, password);
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error while connecting to xfire server", e);
		}
	}

	public void gotConnected(XfireConnection conn) {
	}
}
