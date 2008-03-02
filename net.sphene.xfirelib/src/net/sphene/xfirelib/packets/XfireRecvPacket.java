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

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.attributes.XfireArrayAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValueType;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
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
public class XfireRecvPacket extends XfirePacket<RecvPacketContent> {
	
	private static Logger logger = Logger.getLogger(XfireRecvPacket.class.getName());
	private PacketReader reader;
	
	private int packetId = -1;
	private ByteArrayInputStream inputStream;
	private int length;
	private int numberOfAtts;
	private byte[] bytes;
	
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
		// 3: ??? zero ???
		// 4: packet content type id
		// 5: number of attributes
		
		byte[] buf = new byte[5];
		int c = stream.read(buf, 0, 5);
		
		if(c != 5) {
			throw new IOException("Unable to read 5 bytes - read only {" + c + "}.");
		}
		int length = (int) XfireUtils.convertBytesToInt(buf, 0, 2);
		int packetid = buf[2] & 0xFF;
		int unknown = buf[3] & 0xFF;
		this.packetId = packetid;
		int numberOfAtts = buf[4];
		this.length = length;
		this.numberOfAtts = numberOfAtts;
		logger.fine("Received packet header. length: {" + length +
				"} packetid: {" + packetid + "} unknown: {" + unknown + "} numberofAtts: {" +
				numberOfAtts + "}");
		
		byte[] buf2 = new byte[length-5];
		if(XfireUtils.keepBytes())
			this.bytes = buf2;
		int c2 = stream.read(buf2, 0, length - 5);
		if(c2 < length - 5) {
			logger.severe("Didn't read enough bytes - expected {" + (length-5) + "} but was {" + c2 + "}");
		}
		String debug = XfireUtils.debugBuffer("Of incoming packet.", buf2);
		logger.fine(debug);
		
		RecvPacketContent packetContent = reader.createPacketContentById(packetid);
		if(packetContent == null) {
			return;
		}
		inputStream = new ByteArrayInputStream(buf2);
		setPacketContent(packetContent);
		packetContent.parseContent(this, numberOfAtts);
	}
	
	public InputStream getInputStream() {
		return inputStream;
	}

	public int getPacketId() {
		return packetId;
	}
	
	
	public String readAttributeName() {
		int nameLength = inputStream.read();
		byte buf[] = new byte[nameLength];
		try {
			inputStream.read(buf);
			return new String(buf);
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error while reading attribute name", e);
			return null;
		}
	}
	public byte[] readAttributeValue(int length, boolean ignoreZeroAfterLength) {
		int valueLength = length;
		if(valueLength < 0) {
			valueLength = inputStream.read();
			if(ignoreZeroAfterLength) {
				int zero = inputStream.read();
				logger.finest("Ignoring zero: {" + zero + "}");
			}
		}
		
		byte[] value = new byte[valueLength];
		try {
			inputStream.read(value);
		} catch (IOException e) {
			logger.log(Level.SEVERE, "Error while reading attribute value", e);
			return null;
		}
		return value;
	}
	
	public long readInteger(int length) {
		byte[] b = readAttributeValue(length, false);
		return XfireUtils.convertBytesToInt(b);
	}
	
	public XfireAttribute readAttribute() {
		if(inputStream.available() == 0) {
			return null;
		}
		String name = readAttributeName();
		if(name == null) {
			return null;
		}
		logger.finer("read attribute {" + name + "}");
		int valuetype = inputStream.read();
		if(valuetype == -1) {
			return null;
		}
		try {
			return new XfireAttribute(name, readAttributeValue(valuetype));
		} catch (Exception e) {
			logger.log(Level.SEVERE, "Error while reading attribute value", e);
			return null;
		}
	}

	private XfireAttributeValue readAttributeValue(int valuetypeId) {
		int length;
		XfireAttributeValueType valueType = XfireAttributeValueType.getTypeById(valuetypeId);
		if(valueType == null) {
			throw new RuntimeException("Invalid value type {" + valuetypeId + "}");
		}
		switch(valueType) {
			case STRING:
				// String
				length = (int)readInteger(2);
				break;
			case SHORT_INT:
			case SID:
			case NO_IDEA:
				// small int
				length = valueType.getByteLength();
				break;
			case ARRAY:
				// array
				int valuetypes = inputStream.read();
				int count = (int)readInteger(2);
				XfireAttributeValue[] values = new XfireAttributeValue[count];
				for(int i = 0 ; i < count ; i++) {
					values[i] = readAttributeValue(valuetypes);
				}
				return new XfireArrayAttributeValue(XfireAttributeValueType.getTypeById(valuetypes), values);
			default:
				throw new RuntimeException("Unknown value type " + valueType);
		}
		
		return new XfireScalarAttributeValue(valueType, readAttributeValue(length, false));
	}

	@Override
	public int getAttributeCount() {
		return numberOfAtts;
	}

	@Override
	public int getLength() {
		return length;
	}

	@Override
	public byte[] getBytes() {
		return bytes;
	}
	
}
