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

#include "internal.h"
#include "account.h"
#include "accountopt.h"
#include "blist.h"
#include "conversation.h"
#include "debug.h"
#include "notify.h"
#include "prpl.h" 
#include "proxy.h"
#include "util.h"
#include "version.h" 
#include "sha.h"
#include "x_create.h"

 
int xfire_add_att_name(char *packet,int packet_length, char *att){
	packet[packet_length] = (char)strlen(att);//set att length
	memcpy(packet+packet_length+1,att,strlen(att)); //set attname
	return packet_length+1+strlen(att);
}

void xfire_add_header(char *packet, int length, int type, int atts){
	char buffer[5] = {(char)(length%256),(char)(length/256),(char)type,0,(char)atts};
	memcpy(packet,buffer,5);
}

int xfire_add_array_att(char *packet,int packet_length,char **atts,int number){
	packet[packet_length] = (char)number;
	int i;
	int index = packet_length+1;
	for(i = 0;i < number;i++){
		char temp[2] = {(char)strlen(atts[i]),0x00};
		memcpy(packet+index,temp,2);		
		index+=2;

		memcpy(packet+index,atts[i],strlen(atts[i]));
		index+=strlen(atts[i]);

	}
	
	return index;
}


/**
 *	Create new ip address struct and initialize it with zeros
 *	structure should be freed with xfire_ip_struct_free()
 *
 *	@return			returns newly allocated struct otherwise NULL
 *
*/
xfire_ip_address *
xfire_new_ip_struct()
{
	xfire_ip_address *ip = NULL;
	ip = g_malloc(sizeof(xfire_ip_address));
	if (ip) memset(ip, 0, sizeof(xfire_ip_address));
	return ip;
}


/**
 *	Frees memory associated with ip address struct created with
 *	xfire_new_ip_struct().  No return, if argument is NULL function
 *	just returns
 *
 *	@param		ips		pointer to structure to free
*/
void
xfire_ip_struct_free( xfire_ip_address *ips )
{
	if (!ips) return;
	g_free(ips);
}


/**
 *	Return ip address struct initialized with data supplied by argument
 *	When done with structure, it should be freed with xfire_ip_struct_free()
 *
 *	@param		ip		string ip address to fill struct with ex ("127.0.0.1")
 *
 *	@return				returns NULL if no string provided. Otherwise returns
 *						pointer to struct.  ex: s = struct,
 *						
 *						s->octet = { 127, 0, 0, 1 }
 *						s->ipstr = "127.0.0.1"
 *
 *						Structure should be freed with xfire_ip_struct_free()
*/
xfire_ip_address *
xfire_ipaddr_init(const gchar *ip)
{
	xfire_ip_address *ips = NULL;
	gchar **ss = NULL;
	int i = 0;
	
	if (strlen(ip) <= 0) return NULL;
	ss = g_strsplit(ip,".", 0);
	if (g_strv_length(ss) != 4) {
		g_strfreev(ss);
		return NULL;
	}
	ips = xfire_new_ip_struct();
	if (!ips) {
		g_strfreev(ss);
		return ips;
	}
	for (i=0; i < 4; i++) {
		ips->octet[i] = atoi(ss[i]);
	}
	memcpy((void *)&ips->ipstr, ip, strlen(ip)+1);
	g_strfreev(ss);
	return ips;
}
