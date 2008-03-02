/*
 * File    : GenericSendPacket.java
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

import java.util.LinkedHashMap;
import java.util.Map;

import net.sphene.xfirelib.packets.IAttributeBasedPacketContent;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.content.SendPacketContent;

public abstract class GenericSendPacket extends SendPacketContent implements IAttributeBasedPacketContent {
	
	Map<String, XfireAttribute> attributes = new LinkedHashMap<String, XfireAttribute>();

	@Override
	public void fillPacketContent(XfireSendPacket packet) {
		for(XfireAttribute attr : attributes.values()) {
			packet.writeAttribute(attr);
		}
	}
	
	
	
	public void setAttribute(String name, XfireAttribute attribute) {
		this.attributes.put(name, attribute);
	}
	public void setAttributeValue(String name,
			XfireAttributeValue attributeValue) {
		setAttribute(name, new XfireAttribute(name, attributeValue));
	}
	
	public XfireAttribute getAttribute(String name) {
		return this.attributes.get(name);
	}

	
	public XfireAttribute[] getAttributes() {
		return attributes.values().toArray(new XfireAttribute[0]);
	}
}
