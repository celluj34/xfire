/*
 * File    : XfireUtils.java
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

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.regex.Pattern;

import net.sphene.xfirelib.packets.attributes.XfireArrayAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValueType;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;

public class XfireUtils {
	
	public static boolean DEBUG = false;
	
	public static boolean keepBytes() {
		return DEBUG;
	}
	
	public static long pow(long a, long b) {
		long ret = 1;
		for(int i = 0 ; i < b ; i++) {
			ret *= a;
		}
		return ret;
	}
	
	public static long convertBytesToInt(byte[] bytes, int start, int length) {
		long ret = 0;
		for(int i = start ; i < start + length ; i++) {
			ret += (bytes[i] & 0xff) * pow(256, i - start);
		}
		
		return ret;
	}

	public static long convertBytesToInt(byte[] attrlength) {
		return convertBytesToInt(attrlength, 0, attrlength.length);
	}

	public static XfireAttributeValue getValueSid(byte[] sid) {
		return new XfireScalarAttributeValue(XfireAttributeValueType.SID, sid);
	}
	public static XfireAttributeValue getValueString(String str) {
		return new XfireScalarAttributeValue(XfireAttributeValueType.STRING, str.getBytes());
	}
	public static XfireAttributeValue getValueShortInt(int val) {
		return new XfireScalarAttributeValue(XfireAttributeValueType.SHORT_INT, convertIntToBytes(val, XfireAttributeValueType.SHORT_INT.getByteLength()));
	}
	public static XfireAttributeValue getValueEmptyArray(XfireAttributeValueType elementType) {
		return new XfireArrayAttributeValue(elementType, new XfireAttributeValue[0]);
	}

	public static byte[] convertIntToBytes(int val, int byteLength) {
		byte[] bytes = new byte[byteLength];
		for(int i = 0 ; i < byteLength ; i++) {
			bytes[i] = (byte) (val % 256);
			val /= 256;
		}
		return bytes;
	}
	
	private static Pattern p = Pattern.compile("\\p{Print}");
	/**
	 * I was unable to find a useful method in Character .. (like.. isPrintable :( )
	 * (isLetterOrDigit won't return true for special characters)
	 * @param ch
	 * @return
	 */
	protected static boolean isStringChar(byte b) {
		return p.matcher(Character.toString((char)b)).matches();
	}

	public static String debugBuffer(String debug, byte[] buf2) {
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
			ascii.printf("%1c ", !isStringChar(buf2[i]) ? '#' : buf2[i] & 0xff);
		}
		dec.close();
		hex.close();
		ascii.close();
		
		BufferedReader lr = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(l.toByteArray())));
		BufferedReader rr = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(r.toByteArray())));
		BufferedReader asciir = new BufferedReader(new InputStreamReader(new ByteArrayInputStream(a.toByteArray())));
		
		try {
			ByteArrayOutputStream outputArray = new ByteArrayOutputStream();
			PrintWriter output = new PrintWriter(outputArray);
			output.println("debugging buffer: " + debug);
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
				output.printf("%-" + leftlen + "s || %-" + rightlen + "s || %-" + asciilen + "s\n", leftline, rightline, asciiline);
			}
			output.close();
			return new String(outputArray.toByteArray());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
	}
}
