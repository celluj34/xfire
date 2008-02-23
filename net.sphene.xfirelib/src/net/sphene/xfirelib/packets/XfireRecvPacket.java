/*
 * File    : XfirePacket.java
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
package net.sphene.xfirelib.packets;

import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.content.RecvPacketContent;
import net.sphene.xfirelib.packets.content.XfirePacketContent;

/**
 * Xfire packets consist of two parts: <br>
 * 1.) the xfire packet containing the header (including the packet type id)<br>
 * 2.) the xfire packet content - {@link XfirePacketContent} which actually
 * holds the contents of a packet.
 * 
 * @author kahless
 */
public class XfireRecvPacket {
	
	private static Logger logger = Logger.getLogger(XfireRecvPacket.class.getName());
	private PacketReader reader;
	private RecvPacketContent packetContent;
	
	public XfireRecvPacket(PacketReader reader) {
		this.reader = reader;
	}

	/**
	 * Reads from the input stream a packet. Blocks until the packet is finished.
	 * @param stream
	 */
	public void recvPacket(InputStream stream) throws IOException {
		// read 5 bytes packet header, containing
		// 1. + 2.: packet content length
		// 3: ???
		// 4: packet content type id
		// 5: number of attributes
		
		byte[] buf = new byte[5];
		int c = stream.read(buf, 0, 5);
		
		if(c != 5) {
			throw new IOException("Unable to read 5 bytes - read only {" + c + "}.");
		}
		int length = (int) XfireUtils.convertBytesToInt(buf, 0, 2);
		int packetid = buf[2];
		int numberOfAtts = buf[4];
		logger.fine("Received packet header. length: {" + length +
				"} packetid: {" + packetid + "} numberofAtts: {" +
				numberOfAtts + "}");
		
		byte[] buf2 = new byte[length-5];
		int c2 = stream.read(buf2, 0, length - 5);
		if(c2 < length - 5) {
			logger.severe("Didn't read enough bytes - expected {" + (length-5) + "} but was {" + c2 + "}");
		}
		
		packetContent = reader.createPacketContentById(packetid);
		packetContent.parseContent(buf2, c2, numberOfAtts);
	}
}
