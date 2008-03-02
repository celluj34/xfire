/*
 * File    : PacketDebugger.java
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

import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Rectangle;
import java.io.ByteArrayOutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.lang.reflect.InvocationTargetException;
import java.util.logging.Logger;

import javax.swing.JFrame;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import net.sphene.xfirelib.XfireConnection;
import net.sphene.xfirelib.packets.IAttributeBasedPacketContent;
import net.sphene.xfirelib.packets.XfirePacket;
import net.sphene.xfirelib.packets.XfireUtils;
import net.sphene.xfirelib.packets.attributes.XfireArrayAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttribute;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValue;
import net.sphene.xfirelib.packets.attributes.XfireAttributeValueType;
import net.sphene.xfirelib.packets.attributes.XfireScalarAttributeValue;
import net.sphene.xfirelib.packets.buddylist.Friend;
import net.sphene.xfirelib.packets.content.XfirePacketContent;
import net.sphene.xfirelib.packets.content.recv.GenericRecvPacket;

public class PacketDebugger implements Runnable {
	private JTable table;
	private XfireConnection conn;
	private JTextArea debug;
	TableDebugModel tableDebugModel;
	
	private static Logger logger = Logger.getLogger(PacketDebugger.class.getName());

	public PacketDebugger(XfireConnection conn) throws InterruptedException, InvocationTargetException {
		this.conn = conn;
		XfireUtils.DEBUG = true;
		SwingUtilities.invokeAndWait(this);
	}

	public void run() {
		JFrame frame = new JFrame("Xfire Packet Debugger");
		table = new JTable(tableDebugModel = new TableDebugModel(conn));
		table.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		table.getSelectionModel().addListSelectionListener(new ListSelectionListener() {
			public void valueChanged(ListSelectionEvent e) {
				if(e.getValueIsAdjusting()) {
					return;
				}
				int i = table.getSelectedRow();
				if(i > -1) {
					showDebug(tableDebugModel.getPacket(i));
				}
			}
		});
		
		JScrollPane packetlistscroll = new JScrollPane(table);
		debug = new JTextArea();
		debug.setFont(new Font("Monospaced", Font.PLAIN, 12));
		debug.revalidate();
		

		
		JScrollPane debugscroll = new JScrollPane(debug);
		JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
		splitPane.add(packetlistscroll);
		splitPane.add(debugscroll);
//		frame.getContentPane().setLayout(new FlowLayout());
		frame.getContentPane().add(splitPane);
		frame.setSize(500,500);
		frame.setVisible(true);
	}

	protected void showDebug(XfirePacket<? extends XfirePacketContent> packet) {
		ByteArrayOutputStream out = new ByteArrayOutputStream();
		PrintWriter buf = new PrintWriter(out);
		
		XfirePacketContent content = packet.getPacketContent();
		if(content instanceof IAttributeBasedPacketContent) {
			IAttributeBasedPacketContent attrbased = (IAttributeBasedPacketContent) content;
			XfireAttribute[] attrs = attrbased.getAttributes();
			buf.append("Packet is attribute based. Following Attributes:\n");
			
			for(XfireAttribute attr : attrs) {
				buf.append("Name: {").append(attr.getName()).append('}')
						.append(" Value: {").append(
								String.valueOf(attr.getValue()))
						.append('}');
				if(attr.getValue().getValueType() == XfireAttributeValueType.SID) {
					Friend friend = conn.getBuddyList().getFriendBySid(((XfireScalarAttributeValue)attr.getValue()).getByteValue());
					buf.append(" - Friend: {").append(String.valueOf(friend)).append("}");
				} else if(attr.getValue().getValueType() == XfireAttributeValueType.ARRAY) {
					XfireArrayAttributeValue val = (XfireArrayAttributeValue) attr.getValue();
					if(val.getElementType() == XfireAttributeValueType.SID) {
						buf.append(" - Friends: [");
						for(XfireAttributeValue elval : val.getValues()) {
							Friend friend = conn.getBuddyList().getFriendBySid(((XfireScalarAttributeValue)elval).getByteValue());
							buf.append("{").append(String.valueOf(friend)).append("},");
						}
					}
				}
				buf.append('\n');
			}
			
			buf.append("\n\n=========================================\n\n");
		}
		buf.append(XfireUtils.debugBuffer("", packet.getBytes()));
		
		buf.close();
		
		debug.setText(out.toString());
		debug.scrollRectToVisible(new Rectangle(0,0,0,0));
	}
}
