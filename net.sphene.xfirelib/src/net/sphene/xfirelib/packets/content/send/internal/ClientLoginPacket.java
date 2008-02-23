/*
 * File    : ClientLoginPacket.java
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
package net.sphene.xfirelib.packets.content.send.internal;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.logging.Level;
import java.util.logging.Logger;

import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.content.SendPacketContent;

public class ClientLoginPacket extends SendPacketContent {
	
	final static Logger logger = Logger.getLogger(ClientLoginPacket.class.getName());
	
	private String username;
	private String password;
	private byte[] salt;

	public ClientLoginPacket(String username, String password, byte[] salt) {
		this.username = username;
		this.password = password;
		this.salt = salt;
	}

	@Override
	public void fillPacketContent(XfireSendPacket packet) {
		byte[] username = this.username.getBytes();

		// Username
		packet.addAttributeName("name");
		packet.write(0x01);
		packet.write(username.length);
		packet.write(0x00);
		packet.write(username);
		
		// Password
		packet.addAttributeName("password");
		byte[] pass = cryptPassword();
		packet.write(0x01);
		packet.write(40); // length
		packet.write(0x00);
		packet.write(pass);
		
	}

	@Override
	public int getPacketId() {
		return 1;
	}

	
	public byte[] cryptPassword() {
		String total = username + password + "UltimateArena";
		
		byte[] pwcrypt = crypt(total.getBytes());
		assert pwcrypt.length == 40 : "crypted password has to be 40 chars long.";
		
		byte[] temp = new byte[80];
		System.arraycopy(pwcrypt, 0, temp, 0, pwcrypt.length);
		System.arraycopy(salt, 0, temp, 40, salt.length);
		
		byte[] ret = crypt(temp);
		assert ret.length == 40 : "must be 40 bytes long.";
		
		return ret;
	}
	
	public byte[] crypt(byte [] buf) {
		try {
			MessageDigest sha1;
			sha1 = MessageDigest.getInstance("SHA-1");
			sha1.update(buf);

			byte[] digest = sha1.digest();
			ByteArrayOutputStream stream = new ByteArrayOutputStream();
			PrintStream print = new PrintStream(stream);
			for(byte b : digest) {
				print.printf("%02x", b);
			}
			print.flush();
			return stream.toByteArray();
		} catch (NoSuchAlgorithmException e) {
			logger.log(Level.SEVERE, "Unable to encrypt password", e);
			return null;
		}
	}
}
