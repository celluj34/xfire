/*
 * File    : SwingWindow.java
 * Created : Mar 1, 2008
 * By      : herbert
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

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;

import net.sphene.xfirelib.ConnectionListener;
import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.PacketListener;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.buddylist.BuddyList;
import net.sphene.xfirelib.packets.buddylist.Friend;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.recv.MessageIncoming;
import net.sphene.xfirelib.packets.content.send.MessageOutgoing;

public class SwingWindow implements Runnable, PacketListener {
	
	private XfireConnection conn;
	private JFrame frame;
	private JTextArea area;
	private JTextField input;
	protected BuddyList buddylist;

	public SwingWindow(XfireConnection conn) {
		this.conn = conn;
		
		try {
			SwingUtilities.invokeAndWait(this);
		} catch (Exception e) {
			e.printStackTrace();
		}
		
	}

	public void run() {
		frame = new JFrame("Xfire Test Client");

		area = new JTextArea();
		area.setEditable(false);
		area.setFont(new Font("Monospaced", Font.PLAIN, 12));
		area.revalidate();
		JScrollPane textareascroll = new JScrollPane(area);
		input = new JTextField();
		
		frame.getContentPane().setLayout(new BorderLayout());
		frame.getContentPane().add(textareascroll, BorderLayout.CENTER);
		frame.getContentPane().add(input, BorderLayout.SOUTH);
		frame.pack();
		frame.setSize(new Dimension(1000,500));
		frame.setVisible(true);
		
		input.requestFocus();
		
		
		input.addKeyListener(new KeyListener() {
		
			public void keyTyped(KeyEvent e) {
				if(e.getKeyChar() == '\n') {
					String command = input.getText();
					input.setText("");
					processCommand(command);
					System.out.println("Command: {" + command + "}");
				}
			}
		
			public void keyReleased(KeyEvent e) {
			}
		
			public void keyPressed(KeyEvent e) {
			}
		});
		
		conn.addConnectionListener(new ConnectionListener() {
			public void gotConnected(XfireConnection conn) {
				addMessage("We got connected to the xfire server.");
			}
		});
		buddylist = conn.getBuddyList();
		conn.addPacketListener(this);
	}
	
	protected void processCommand(String command) {
		addInputmessage(command);
		String[] args = command.split(" ");
		if("buddylist".equals(args[0])) {
			printBuddyList();
		} else if("sendmessage".equals(args[0])) {
			if(args.length < 3) {
				addMessage("Usage: sendmessage <username> <message ...>");
				return;
			}
			String username = args[1];
			StringBuffer buf = new StringBuffer();
			
			for(int i = 2 ; i < args.length ; i++) buf.append(' ').append(args[i]);
			
			String message = buf.substring(1);
			Friend friend = buddylist.getFriendByUserName(username);
			if(friend == null) {
				addMessage("Invalid user name: {" + username + "}");
				return;
			}
			conn.send(new MessageOutgoing(friend, message));
			addMessage("Sent message to {" + username + "}");
		} else {
			addMessage("Invalid command {" + args[0] + "}");
		}
	}
	
	public void printBuddyList() {
		addMessage("Printing buddy list ...");
		addMessage(buddylist.getBuddyListTable());
	}

	public void addMessage(String message) {
		if(message.contains("\n")) {
			String[] lines = message.split("\n");
			for(String line : lines) {
				addMessage(line);
			}
			return;
		}
		area.append("=== " + message + "\n");
	}
	
	public void addInputmessage(String command) {
		area.append(">>> " + command + "\n");
	}

	public void receivedPacket(XfireRecvPacket packet) {
		RecvPacketContent content = packet.getPacketContent();
		if(content instanceof MessageIncoming) {
			MessageIncoming msg = (MessageIncoming) content;
			Friend friend = msg.getFriend(buddylist);
			addMessage("Received a message from: {" + friend.getName() + "}: " + msg.getMessage());
		}
	}

	public void sendingPacket(XfireSendPacket packet) {
		// TODO Auto-generated method stub
		
	}

	public void sentPacket(XfireSendPacket packet) {
		// TODO Auto-generated method stub
		
	}

}
