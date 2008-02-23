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

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.attributes.XfireArrayAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
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
		int packetid = buf[2] & 0xFF;
		int unknown = buf[3] & 0xFF;
		this.packetId = packetid;
		int numberOfAtts = buf[4];
		logger.fine("Received packet header. length: {" + length +
				"} packetid: {" + packetid + "} unknown: {" + unknown + "} numberofAtts: {" +
				numberOfAtts + "}");
		
		byte[] buf2 = new byte[length-5];
		int c2 = stream.read(buf2, 0, length - 5);
		if(c2 < length - 5) {
			logger.severe("Didn't read enough bytes - expected {" + (length-5) + "} but was {" + c2 + "}");
		}
		debugBuffer(buf2);
		
		RecvPacketContent packetContent = reader.createPacketContentById(packetid);
		if(packetContent == null) {
			return;
		}
		inputStream = new ByteArrayInputStream(buf2);
		packetContent.parseContent(this, numberOfAtts);
		setPacketContent(packetContent);
	}
	
	private void debugBuffer(byte[] buf2) {
		ByteArrayOutputStream l = new ByteArrayOutputStream();
		ByteArrayOutputStream r = new ByteArrayOutputStream();
		ByteArrayOutputStream a = new ByteArrayOutputStream();
		PrintWriter dec = new PrintWriter(l);
		PrintWriter hex = new PrintWriter(r);
		PrintWriter ascii = new PrintWriter(a);
		int perline = 5;
		for(int i = 0 ; i < buf2.length ; i++) {
			if( i % perline == 0 && i > 0 ) {
				dec.append('\n');
				hex.append('\n');
				ascii.append('\n');
			}
			dec.printf("%3d ", buf2[i] & 0xff);
			hex.printf("%2x ", buf2[i]);
			ascii.printf("%1c ", buf2[i] < 15 ? 0 : buf2[i] & 0xff);
		}
		dec.close();
		hex.close();
		ascii.close();
		
		BufferedReader lr = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(l.toByteArray())));
		BufferedReader rr = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(r.toByteArray())));
		BufferedReader asciir = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(a.toByteArray())));
		
		try {
			System.out.println("debugging buffer");
			while(true) {
				String leftline = lr.readLine();
				String rightline = rr.readLine();
				String asciiline = asciir.readLine();
				if(leftline == null || rightline == null) {
					break;
				}
				int leftlen = perline * 4;
				int rightlen = perline * 3;
				int asciilen = perline * 2;
				System.out.printf("%-" + leftlen + "s || %-" + rightlen + "s || %-" + asciilen + "s\n", leftline, rightline, asciiline);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
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
	
	public XfireAttribute readAttribute() {
		String name = readAttributeName();
		logger.finer("read attribute {" + name + "}");
		int valuetype = inputStream.read();
		return new XfireAttribute(name, readAttributeValue(valuetype));
//		int length;
//		if(lengthtype == 1) {
//			byte[] attrlength = readAttributeValue(1, false);
//			length = (int) XfireUtils.convertBytesToInt(attrlength);
//			int zero = inputStream.read();
//			logger.finest("Ignoring zero in readAttribute {" + zero + "}");
//		} else if(lengthtype == 2) {
//			length = 4;
//		} else if(lengthtype == 3) {
//			length = 16;
//		} else if(lengthtype == 4) {
//			length = 3;
//		} else if(lengthtype == 6) {
//			length = 21;
//		} else {
//			logger.severe("Unknown value length type {" + lengthtype + "}");
//			return null;
//		}
//		int zero = inputStream.read();
//		logger.finest("Ignoring zero in readAttribute {" + zero + "}");
//		byte[] value = readAttributeValue(length, false);
//		return new XfireAttribute(name, value);
//		return null;
	}
	
	private void readZero() {
		int zero = inputStream.read();
		assert zero == 0 : "reading zero but got {" + zero + "}";
	}

	private XfireAttributeValue readAttributeValue(int valuetype) {
		int length;
		switch(valuetype) {
			case 1:
				// String
				length = inputStream.read();
				readZero();
				break;
			case 2:
				// small int
				length = 4;
				break;
			case 3:
				// ???
				length = 16;
				break;
			case 4:
				// array
				int valuetypes = inputStream.read();
				int count = inputStream.read();
				readZero();
				XfireAttributeValue[] values = new XfireAttributeValue[count];
				for(int i = 0 ; i < count ; i++) {
					values[0] = readAttributeValue(valuetypes);
				}
				return new XfireArrayAttributeValue(values);
			default:
				throw new RuntimeException("Invalid value type {" + valuetype + "}");
		}
		
		return new XfireScalarAttributeValue(readAttributeValue(length, false));
	}
	
}
