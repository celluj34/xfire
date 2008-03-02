/*
 * File    : XfireAttributeValueType.java
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
package net.sphene.xfirelib.packets.attributes;

public enum XfireAttributeValueType {
	
	STRING    (1, -1),
	SHORT_INT (2, 4),
	SID       (3, 16),
	ARRAY     (4, -1),
	NO_IDEA   (5, 1);
	
	
	private int typeId;
	private int bytelength;
	XfireAttributeValueType(int typeId, int bytelength) {
		this.typeId = typeId;
		this.bytelength = bytelength;
	}
	
	public int getTypeId() {
		return this.typeId;
	}
	
	/**
	 * Returns the byte in length of this attribute, or -1 if it is variable.
	 */
	public int getByteLength() {
		return bytelength;
	}
	
	public static XfireAttributeValueType getTypeById(int typeId) {
		for (XfireAttributeValueType type : XfireAttributeValueType.values()) {
			if (type.getTypeId() == typeId) {
				return type;
			}
		}
		return null;
	}
}
