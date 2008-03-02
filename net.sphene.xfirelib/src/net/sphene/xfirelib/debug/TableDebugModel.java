/*
 * File    : TableDebugModel.java
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
package net.sphene.xfirelib.debug;

import java.util.ArrayList;
import java.util.List;

import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.AbstractTableModel;

import net.sphene.xfirelib.ConnectionListener;
import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.PacketListener;
import net.sphene.xfirelib.packets.XfirePacket;
import net.sphene.xfirelib.packets.XfireRecvPacket;
import net.sphene.xfirelib.packets.XfireSendPacket;
import net.sphene.xfirelib.packets.content.XfirePacketContent;

public class TableDebugModel extends AbstractTableModel implements ConnectionListener, PacketListener {
	private static final long serialVersionUID = 1L;
	
	private String[] columnNames = new String[] { "Direction", "Time", "Type", "Content Class", "Length", "Attrs", };
	
	private List<DebugPacketWrapper> packets = new ArrayList<DebugPacketWrapper>();

	public TableDebugModel(XfireConnection conn) {
		conn.addConnectionListener(this);
		conn.addPacketListener(this);
	}

	public int getColumnCount() {
		return columnNames.length;
	}
	
	@Override
	public String getColumnName(int column) {
		return columnNames[column];
	}

	public int getRowCount() {
		return packets.size();
	}

	public Object getValueAt(int row, int col) {
		DebugPacketWrapper packetWrapper = packets.get(row);
		XfirePacket<? extends XfirePacketContent> packet = packetWrapper.getPacket();
		switch(col) {
		case 0: return (packet instanceof XfireRecvPacket ? "Incoming" : "Outgoing");
		case 1: return packetWrapper.getTimeString();
		case 2: return packet.getPacketContent().getPacketId();
		case 3: return packet.getPacketContent().getClass().getSimpleName();
		case 4: return packet.getLength();
		case 5: return packet.getAttributeCount();
		}
		return null;
	}

	public void gotConnected(XfireConnection conn) {
	}

	public void receivedPacket(XfireRecvPacket packet) {
		newPacket(packet);
	}
	public void sendingPacket(XfireSendPacket packet) {
	}
	public void sentPacket(XfireSendPacket packet) {
		newPacket(packet);
	}

	
	protected void newPacket(XfirePacket<? extends XfirePacketContent> packet) {
		packets.add(new DebugPacketWrapper(packet));
		TableModelListener[] listeners = getListeners(TableModelListener.class);
		TableModelEvent event = new TableModelEvent(this);//, packets.size()-1);
		for(TableModelListener listener : listeners) {
			listener.tableChanged(event);
		}
	}

	public XfirePacket<? extends XfirePacketContent> getPacket(int i) {
		return packets.get(i).getPacket();
	}
	
	public DebugPacketWrapper getPacketWrapper(int i) {
		return packets.get(i);
	}

}
