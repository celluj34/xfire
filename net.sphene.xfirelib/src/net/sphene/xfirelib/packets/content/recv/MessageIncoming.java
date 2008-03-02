/*
 * File    : MessageIncoming.java
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
package net.sphene.xfirelib.packets.content.recv;

import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
import net.sphene.xfirelib.packets.buddylist.BuddyList;
import net.sphene.xfirelib.packets.buddylist.Friend;
import net.sphene.xfirelib.packets.content.MessageType;

public class MessageIncoming extends GenericRecvPacket {

	@Override
	public int getPacketId() {
		return 133;
	}
	
	public byte[] getSid() {
		return ((XfireScalarAttributeValue)getAttributeValue("sid")).getByteValue();
	}
	
	public String getMessage() {
		XfireScalarAttributeValue im = ((XfireScalarAttributeValue)getAttributeValue("im"));
		if(im == null) {
			return null;
		}
		return im.getStringValue();
	}
	
	public MessageType getMessageType() {
		XfireScalarAttributeValue msgtype = (XfireScalarAttributeValue) getAttributeValue("msgtype");
		if(msgtype == null) return null;
		return MessageType.getMessageTypeById(msgtype.getIntValue());
	}
	
	public Friend getFriend(BuddyList buddylist) {
		return buddylist.getFriendBySid(getSid());
	}

}
