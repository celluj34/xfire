/*
 * File    : MessageOutgoing.java
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
package net.sphene.xfirelib.packets.content.send;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import net.sphene.xfirelib.packets.XfireUtils;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValueType;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
import net.sphene.xfirelib.packets.buddylist.Friend;
import net.sphene.xfirelib.packets.content.MessageType;

public class MessageOutgoing extends GenericSendPacket {
	
	private static class Sid {
		private byte[] sid;

		public Sid(byte[] sid) {
			this.sid = sid;
		}
		
		@Override
		public boolean equals(Object obj) {
			if(!(obj instanceof Sid)) {
				return super.equals(obj);
			}
			return Arrays.equals(this.sid, ((Sid)obj).sid);
		}
		
		@Override
		public int hashCode() {
			return Arrays.hashCode(sid);
		}
	}
	
	private static Map<Sid, Integer> imindexes = new HashMap<Sid, Integer>();
	
	public MessageOutgoing(Friend friend, String message) {
		byte[] sid = friend.getSid();
		int imindex = getNewImIndex(sid);
		setAttributeValue("sid", XfireUtils.getValueSid(sid));
		setAttributeValue("peermsg", new XfireScalarAttributeValue(XfireAttributeValueType.NO_IDEA, new byte[] { 03 }));
		setAttributeValue("msgtype", XfireUtils.getValueShortInt(MessageType.CHAT.getTypeId()));
		setAttributeValue("imindex", XfireUtils.getValueShortInt(imindex));
		setAttributeValue("im", XfireUtils.getValueString(message));
	}

	private static synchronized int getNewImIndex(byte[] sidbyte) {
		Sid sid = new Sid(sidbyte);
		Integer imindex = imindexes.get(sid);
		if(imindex == null) {
			imindex = 1;
		}
		imindexes.put(sid, imindex+1);
		return imindex;
	}

	@Override
	public int getPacketId() {
		return 2;
	}

}
