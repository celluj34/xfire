/*
 * File    : BuddyList.java
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
package net.sphene.xfirelib.packets.buddylist;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

import net.sphene.xfirelib.ConnectionListener;
import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.PacketListener;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.recv.BuddyListNamesPacket;
import net.sphene.xfirelib.packets.content.recv.BuddyListOnlinePacket;
import net.sphene.xfirelib.packets.content.recv.RecvStatusMessagePacket;

/**
 * The buddy list keeps record of all friends and their status.
 * 
 * @author kahless
 */
public class BuddyList implements PacketListener, ConnectionListener {
	
	@SuppressWarnings("unused")
	private XfireConnection conn;

	private List<Friend> friends = new ArrayList<Friend>();

	public BuddyList(XfireConnection conn) {
		this.conn = conn;
		conn.addConnectionListener(this);
		conn.addPacketListener(this);
	}
	

	public void receivedPacket(XfireRecvPacket packet) {
		RecvPacketContent content = packet.getPacketContent();
		if(content instanceof BuddyListNamesPacket) {
			receivedBuddyList((BuddyListNamesPacket) content);
		} else if(content instanceof BuddyListOnlinePacket) {
			receivedBuddyListOnline((BuddyListOnlinePacket) content);
		} else if(content instanceof RecvStatusMessagePacket) {
			receivedStatusMessagePacket((RecvStatusMessagePacket) content);
		}
	}

	public Friend getFriendBySid(byte[] sid) {
		for(Friend friend : friends) {
			if(friend.hasSid(sid)) {
				return friend;
			}
		}
		return null;
	}
	
	public Friend getFriendByUserId(int userId) {
		for(Friend friend : friends) {
			if(friend.getUserid() == userId) {
				return friend;
			}
		}
		return null;
	}

	private void receivedBuddyList(BuddyListNamesPacket packet) {
		XfireAttributeValue[] friends = packet.getArrayAttributeValue("friends");
		XfireAttributeValue[] nicks = packet.getArrayAttributeValue("nick");
		XfireAttributeValue[] userid = packet.getArrayAttributeValue("userid");
		
		assert friends.length == nicks.length;
		assert nicks.length == userid.length;
		
		int count = friends.length;
		for (int i = 0; i < count; i++) {
			Friend friend = new Friend(
					((XfireScalarAttributeValue)userid[i]).getIntValue(),
					((XfireScalarAttributeValue)friends[i]).getStringValue(),
					((XfireScalarAttributeValue)nicks[i]).getStringValue());
			this.friends.add(friend);
		}
		System.out.println(getBuddyListTable());
	}

	private void receivedBuddyListOnline(BuddyListOnlinePacket content) {
		XfireAttributeValue[] userids = content.getArrayAttributeValue("userid");
		XfireAttributeValue[] sid = content.getArrayAttributeValue("sid");
		
		assert userids.length == sid.length;
		
		int count = userids.length;
		for(int i = 0 ; i < count ; i++) {
			Friend friend = getFriendByUserId(((XfireScalarAttributeValue)userids[i]).getIntValue());
			if(friend != null) {
				friend.setSid(((XfireScalarAttributeValue)sid[i]).getByteValue());
			}
		}
		
		System.out.println(getBuddyListTable());
	}


	private void receivedStatusMessagePacket(RecvStatusMessagePacket content) {
		XfireAttributeValue[] sids = content.getArrayAttributeValue("sid");
		XfireAttributeValue[] msgs = content.getArrayAttributeValue("msg");
		
		assert sids.length == msgs.length;
		
		int count = sids.length;
		for(int i = 0 ; i < count ; i++) {
			Friend friend = getFriendBySid(((XfireScalarAttributeValue)sids[i]).getByteValue());
			if(friend != null) {
				friend.setStatusMessage(((XfireScalarAttributeValue)msgs[i]).getStringValue());
			}
		}
		
		System.out.println(getBuddyListTable());
	}

	
	public String getBuddyListTable() {
		ByteArrayOutputStream out = new ByteArrayOutputStream();
		PrintStream print = new PrintStream(out);
		
		
		print.println(" ======================= Buddy List =========================");
		
		print.printf("%2s%20s | %20s | %10s | %30s | %5s | %5s\n", "", "User Name", "Nick",
				"UserId", "Status Msg", "GameId", "GameId2");
		
		for(Friend friend : friends) {
			print.printf("%2s%20s | %20s | %10d | %30s | %5d | %5d\n",
					friend.isOnline() ? "*" : "",
					friend.getName(),
					friend.getNick(),
					friend.getUserid(),
					friend.getStatusMessage(),
					0,
					0);
		}
		
		print.println(" ============================================================");
		
		print.close();
		return new String(out.toByteArray());
	}


	public Friend getFriendByUserName(String username) {
		for(Friend friend : friends) {
			if(username.equals(friend.getName())) {
				return friend;
			}
		}
		return null;
	}


	public void gotConnected(XfireConnection conn) {
		// clear friends list
		friends = new ArrayList<Friend>();
	}


	public void sendingPacket(XfireSendPacket packet) {
	}


	public void sentPacket(XfireSendPacket packet) {
	}
}
