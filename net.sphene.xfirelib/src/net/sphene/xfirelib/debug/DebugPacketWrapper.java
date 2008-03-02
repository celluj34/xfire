/*
 * File    : DebugPacketWrapper.java
 * Created : Mar 2, 2008
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
package net.sphene.xfirelib.debug;

import java.text.SimpleDateFormat;

import net.sphene.xfirelib.packets.XfirePacket;
import net.sphene.xfirelib.packets.content.XfirePacketContent;

public class DebugPacketWrapper {
	private XfirePacket<? extends XfirePacketContent> packet;
	private long time;
	private static SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

	public DebugPacketWrapper(XfirePacket<? extends XfirePacketContent> packet) {
		this.packet = packet;
		this.time = System.currentTimeMillis();
	}
	
	public XfirePacket<? extends XfirePacketContent> getPacket() {
		return packet;
	}
	public long getTime() {
		return time;
	}
	public String getTimeString() {
		return format.format(time);
	}
}
