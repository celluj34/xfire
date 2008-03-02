/*
 * File    : XfireSendPacket.java
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

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.attributes.XfireArrayAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
import net.sphene.xfirelib.packets.content.SendPacketContent;

public class XfireSendPacket extends XfirePacket<SendPacketContent> {
	
	private static Logger logger = Logger.getLogger(XfireSendPacket.class.getName());
	
	private ByteArrayOutputStream outputStream;

	private int attributeCounter = 0;

	private int length;

	private byte[] bytes;

	public XfireSendPacket(SendPacketContent packetContent) {
		super.setPacketContent(packetContent);
		outputStream = new ByteArrayOutputStream();
	}
	
	
	public int getAttributeCount() {
		return attributeCounter;
	}
	
	public int getPacketLength() {
		return 5 + outputStream.size();
	}
	
	public OutputStream getOutputStream() {
		return outputStream;
	}

	public void addAttributeName(String name) {
		attributeCounter++;
		// TODO encoding ?
		byte[] buf = name.getBytes();
		this.write(buf.length);
		this.write(buf);
	}
	
	public void writeAttribute(XfireAttribute attr) {
		// First write the name ..
		XfireAttributeValue val = attr.getValue();
		addAttributeName(attr.getName());
		
		// Write out the value type
		this.write(val.getValueType().getTypeId());
		
		// Write the value.
		writeAttributeValue(val);
	}


	private void writeAttributeValue(XfireAttributeValue val) {
		switch(val.getValueType()) {
		case STRING:
			// First write the length ..
			byte[] bytes = ((XfireScalarAttributeValue) val).getByteValue();
			this.write(XfireUtils.convertIntToBytes(bytes.length, 2));
		case SHORT_INT:
		case SID:
		case NO_IDEA:
			this.write(((XfireScalarAttributeValue) val).getByteValue());
			break;
		case ARRAY:
			XfireArrayAttributeValue array = (XfireArrayAttributeValue)val;
			int typeId = array.getElementType().getTypeId();
			this.write(typeId);
			this.write(XfireUtils.convertIntToBytes(array.getValues().length, 2));
			for(XfireAttributeValue elementvalue : array.getValues()) {
				writeAttributeValue(elementvalue);
			}
			break;
		default:
			throw new RuntimeException("Unsupported value type: "
					+ val.getValueType());
		}
	}
	

	public void write(int b) {
		outputStream.write(b);
	}
	
	public void write(byte b[]) {
		try {
			outputStream.write(b);
		} catch (IOException e) {
			logger.log(Level.SEVERE, "error while writing to output stream", e);
		}
	}
	
	public void write(byte b[], int off, int len) {
		outputStream.write(b, off, len);
	}
	
	
	
	
	public void sendPacket(OutputStream sendStream) throws IOException {
		SendPacketContent content = getPacketContent();
		content.fillPacketContent(this);
		logger.finer("Sending packet ... {" + content.getClass().getName() + "}");
		int len = getPacketLength();
		this.length = len;
		byte buf[] = new byte[len];
		buf[0] = (byte) (len % 256);
		buf[1] = (byte) (len / 256);
		buf[2] = (byte) content.getPacketId();
		buf[3] = (byte) 0x00;
		buf[4] = (byte) getAttributeCount();
		
		byte[] oldbuf = outputStream.toByteArray();
		if(XfireUtils.keepBytes())
			this.bytes = oldbuf;
		String debug = XfireUtils.debugBuffer("Of outgoing packet. packet id{" + content.getPacketId() + "} attrs {" + getAttributeCount() + "}", oldbuf);
		logger.fine(debug);
		System.arraycopy(oldbuf, 0, buf, 5, oldbuf.length);
		
		sendStream.write(buf);
		sendStream.flush();
		
	}


	@Override
	public int getLength() {
		return this.length;
	}

	@Override
	public byte[] getBytes() {
		return bytes;
	}
}
