/*
 * File    : Friend.java
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

import java.util.Arrays;

public class Friend {
	private int userid;
	private byte[] sid;
	private String name;
	private String nick;
	private String statusMessage;
	
	
	public Friend(int userid, String name, String nick) {
		this.userid = userid;
		this.name = name;
		this.nick = nick;
	}
	
	
	public byte[] getSid() {
		return sid;
	}
	public void setSid(byte[] sid) {
		this.sid = sid;
	}
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getNick() {
		return nick;
	}
	public void setNick(String nick) {
		this.nick = nick;
	}
	public String getStatusMessage() {
		return statusMessage;
	}
	public void setStatusMessage(String statusMessage) {
		this.statusMessage = statusMessage;
	}


	public boolean hasSid(byte[] sid) {
		return Arrays.equals(this.sid, sid);
	}


	public int getUserid() {
		return userid;
	}


	public void setUserid(int userid) {
		this.userid = userid;
	}


	public boolean isOnline() {
		return getSid() != null;
	}
	
	@Override
	public String toString() {
		return "User Id: {" + getUserid() + "} Name: {" + getName() + "} Nick: {" + getNick() + "}";
	}
	
}
