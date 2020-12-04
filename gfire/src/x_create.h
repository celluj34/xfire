/*
 * gaim - Xfire Protocol Plugin
 *
 * Copyright (C) 2000-2001, Beat Wolf <asraniel@fryx.ch>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

struct xfire_ip_address
{
	guint8	octet[4];
	char	ipstr[16];
} typedef xfire_ip_address;

int xfire_add_att_name(char *packet,int packet_length, char *att);
void xfire_add_header(char *packet, int length, int type, int atts);
int xfire_add_array_att(char *packet,int packet_length,char **atts,int number);

/* create new ip address struct */
xfire_ip_address *xfire_new_ip_struct();

/* frees memory associated with ip address struct */
void xfire_ip_struct_free( xfire_ip_address *ips );

/* return new ip address struct with data provided py ip string */
xfire_ip_address *xfire_ipaddr_init(const gchar *ip);
