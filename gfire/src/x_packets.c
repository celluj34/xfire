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

#include "x_utils.h"
#include "x_create.h"

/*create a im and return its length*/
int xfire_make_im(char *local_packet, char *sid, char *message, int imindex){
	int length = 68+strlen(message);
	int index = 0;
	
	xfire_add_header(local_packet, length, 2, 2);/*add header*/
	index += 5;	

	index = xfire_add_att_name(local_packet,index, "sid");/*add sid*/
	local_packet[index] = 0x03;	
	index++;
	memcpy(local_packet+index,sid,16);
	index += 16;

	index = xfire_add_att_name(local_packet,index, "peermsg");/*add peermsg 05 03*/
	local_packet[index] = 0x05;
	local_packet[index+1] = 0x03;
	index +=2;
	
	index = xfire_add_att_name(local_packet,index, "msgtype");/*add msgtype 02 00 00 00 00*/
	local_packet[index] = 0x02;
	local_packet[index+1] = 0x00;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index +=5;
	
	index = xfire_add_att_name(local_packet,index, "imindex");/*add peermsg 02 imindex 00 00 00*/
	local_packet[index] = 0x02;
	local_packet[index+1] = (char)imindex;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index +=5;

	index = xfire_add_att_name(local_packet,index, "im");/*add im 01 message_length 00*/
	local_packet[index] = 0x01;
	local_packet[index+1] = (char)strlen(message);
	local_packet[index+2] = 0x00;
	index +=3;
	
	memcpy(local_packet+index,message,strlen(message));
	index += strlen(message);

	return index;
}

/*
*Sends a nickname change to the server
*/
int xfire_send_alias_change(char *local_packet, char *name){
	int length = 0;
	int index = 0;
	if(name != NULL){
		length = 13+strlen(name);
		xfire_add_header(local_packet, length, 14, 1);/*add header*/
		index += 5;
		
		index = xfire_add_att_name(local_packet,index, "nick");/*add im 01 message_length 00*/
		local_packet[index] = 0x01;
		local_packet[index+1] = (char)strlen(name);
		local_packet[index+2] = 0x00;
		index += 3;
		memcpy(local_packet+index,name,strlen(name));
		index+=strlen(name);
		return index;
	}
	return 0;
}

/*
*Sends the packet when we join a game or leave it (gameid 00 00)
*/
int xfire_join_game(char *local_packet, int game, int port, const char *ip){
	int length = 17;
	int index = 0;
	xfire_ip_address *ips = NULL;
	
	xfire_add_header(local_packet, length, 4, 1);/*add header */
	index += 5;	

	index = xfire_add_att_name(local_packet,index, "gameid");/*add gameid */
	local_packet[index] = 0x02;
	local_packet[index+2] = game/256;
	local_packet[index+1] = game-(((int)(game/256))*256);
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index += 5;
	length = 37;
	xfire_add_header(local_packet, length, 4, 3); /* update num attr */
	if ( ip && strlen(ip) ) {
	   ips = xfire_ipaddr_init(ip);
	} else {
	   ips = xfire_ipaddr_init("0.0.0.0");
	   port = 0;
	}
	index = xfire_add_att_name(local_packet, index, "gip");
	local_packet[index++] = 0x02;
	local_packet[index++] = ips->octet[3];
	local_packet[index++] = ips->octet[2];
	local_packet[index++] = ips->octet[1];
	local_packet[index++] = ips->octet[0];
	index = xfire_add_att_name(local_packet, index, "gport");
	local_packet[index++] = 0x02;
	local_packet[index++] = port - (((int)(port/256))*256);
	local_packet[index++] = port/256;
	local_packet[index++] = 0x00;
	local_packet[index++] = 0x00;
	xfire_ip_struct_free(ips);
//	printf("indexval = %d\n", index);
	return index;
}

/*
 * sends this packet to the server:
 * magicNumber 00 type(18) 00 numberOfAtts //skins and versions of skins are 1 
 * attribute_length 'skin' 04 01 numberOfSkins 00 lengthOfNextSkin 00 nameOfSkin LengthOfNextSkin 00 nameOfSkin (..andSoOn..) 
 * attribute_length  'version' 04 02 numberOfSkins 00 (forEachSkin){ 01 00 00 00 } 12 00 03 00 01 
 * attribute_length 'version' 02 versionNumber 00 00 00
 *
 */
int xfire_open_connection2(char *packet, int version){
	int length = 51;
	int index = 0;
	int skins = 2;
	int i;
	
	xfire_add_header(packet, length, 18, 2);/*add header*/
	index += 5;	

	index = xfire_add_att_name(packet,index, "skin");/*add skin*/
	packet[index] = 0x04;
	packet[index+1] = 0x01;
	packet[index+2] = (char)skins;
	packet[index+3] = 0x00;
	packet[index+4] = strlen("Standard");
	packet[index+5] = 0x00;
	index += 6;
	
	memcpy(packet+index,"Standard",strlen("Standard"));/*add first skin name*/
	index += strlen("Standard");
	
	packet[index] = strlen("XFire");
	packet[index+1] = 0x00;
	index += 2;
	
	memcpy(packet+index,"XFire",strlen("XFire"));/*add second skin name*/
	index += strlen("XFire");	
	
	index = xfire_add_att_name(packet,index, "version");/*add version of skins*/
	packet[index] = 0x04;
	packet[index+1] = 0x02;
	packet[index+2] = (char)skins;
	packet[index+1] = 0x00;
	index += 4;
	
	for(i = 0;i < skins;i++){/*(forEachSkin){ 01 00 00 00 }*/
		packet[index] = 0x01;
		packet[index+1] = 0x00;
		packet[index+2] = 0x00;
		packet[index+3] = 0x00;
		index += 4;
	}
	
	packet[index] = 0x12;
	packet[index+1] = 0x00;
	packet[index+2] = 0x03;
	packet[index+3] = 0x00;
	packet[index+4] = 0x01;
	index +=5;
	
	index = xfire_add_att_name(packet,index, "version");/*add xfire version*/
	packet[index] = 0x02;
	packet[index+1] = (char) version;//protocol version
	packet[index+2] = 0x00;
	packet[index+3] = 0x00;
	packet[index+4] = 0x00;
	index += 5;
	
	return index;
}

/*
create the packet to accept a invitation
*/
int xfire_invitation_accept(char *local_packet, char *name){

	int length = 13 + strlen(name);
	int index = 0;
	
	xfire_add_header(local_packet, length, 7, 1);
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "name");
	local_packet[index] = 0x01;
	local_packet[index+1] = (char)strlen(name);
	local_packet[index+2] = 0x00;
	index += 3;
	
	memcpy(local_packet+index,name,strlen(name));
	index += strlen(name);
	return index;
}

/*
create the packet to deny a invitation
*/
int xfire_invitation_deny(char *local_packet, char *name){

	int length = 13 + strlen(name);
	int index = 0;
	
	xfire_add_header(local_packet, length, 8, 1);
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "name");
	local_packet[index] = 0x01;
	local_packet[index+1] = (char)strlen(name);
	local_packet[index+2] = 0x00;
	index += 3;
	
	memcpy(local_packet+index,name,strlen(name));
	index += strlen(name);
	return index;
}

int xfire_add_new_buddy(char *buffer, char *name){	
	char message[] = "please add me";
	int length;
	int index = 0;
	char *temp2[] = {message};	
	char *temp[] = {name};
	
	length = 13+strlen(name)+7+strlen(message);
	
	xfire_add_header(buffer, length, 6, 2);
	index += 5;
	
	index = xfire_add_att_name(buffer,index, "name");
	
	index = xfire_add_array_att(buffer,index,temp,1);

	index = xfire_add_att_name(buffer,index, "msg");	
	
	index = xfire_add_array_att(buffer,index,temp2,1);
	return index;
}

int xfire_delete_buddy(char *buffer, char *name){
	char userid[4];
	int index = 0;
	
	xfire_add_header(buffer, 17, 9, 1);
	index += 5;
	index = xfire_add_att_name(buffer,index, "userid");	
	
	decodeString(userid, name);

	buffer[index] = 0x02;
	index++;
	memcpy(buffer+index,userid,4);
	return index+4; /*size is always 17*/
}

int xfire_create_auth(char *local_packet,char *salt,char *passwd,char *name){
	/*
	 * packet_length 00 type(01) 00 numberOfAtts
	 * attribute_length 'name'  usernameLength_length usernameLength 00 username
	 * attribute_length 'password'  passwdLength_length passwdLength 00 cryptedPassword
	 */
	char secret[] = "UltimateArena";		 /*Secret string that is used to hash the passwd*/	
	
	char sha_string[40];
	char hash_it[100];
	char hash_final[81];
	
	int length = 97+strlen(name); /*Packet length is 97 + username length, 107 (total)*/ 
	
	int index = 0;
	
	xfire_add_header(local_packet, length, 1, 3); /*add header*/ 
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "name");/*add name*/
	local_packet[index] = 0x01; 			/*username length length*/
	local_packet[index+1] = (char)strlen(name); 	/*username length*/
	local_packet[index+2] = 0x00;
	
	index += 3;
	
	memcpy(local_packet+index,name,strlen(name)); 	/* add username */
	index += strlen(name);
	
	index = xfire_add_att_name(local_packet,index, "password");
	local_packet[index] = 0x01; 			/*hashed passwd length length*/
	local_packet[index+1] = 0x28; 			/*hashed passwd length, always 40 (SHA1)*/
	local_packet[index+2] = 0x00;
	index += 3;
	
	hash_it[0] = 0;
	strcat(hash_it,name);				/*create string: name+passwd+secret*/
	strcat(hash_it,passwd);
	strcat(hash_it,secret);
	hashSha1(hash_it,sha_string);			/*hash the string*/
	
	memcpy(hash_final,sha_string,40);		/*mix it with the salt and rehash*/
	memcpy(hash_final+40,salt,40);
	
	hash_final[80] = 0x00; 				/*terminate the string*/

	hashSha1(hash_final,sha_string);
		
	memcpy(local_packet+index,sha_string,sizeof(sha_string));/*insert the hash of the passwd*/
	index += sizeof(sha_string);
	
	/* added 09-08-2005 difference in login packet */ 
	index = xfire_add_att_name(local_packet,index, "flags");/*add flags*/ 
	local_packet[index]=0x02; 
	index++; 
	
	// run memset once, fill 25 char's with 0's this is from a packet capture 
	// they tack on "flags" + 4 bytes that are 0x00 + "sid" + 16 bytes that are 0x00 
	
	memset(local_packet + index, 0x00, 25); 
	index+=4; 
	index = xfire_add_att_name(local_packet,index, "sid");/*add sid*/ 
	local_packet[index] = 0x03; 
	
	// rest of packet is 16 bytes filled with 0x00 
	index+= 17; 
	
	return index;
}

/*Send away message to the server*/
int xfire_create_away_old(char *local_packet, char *away){
	int length = 29+strlen(away);
	int index = 0;
	xfire_add_header(local_packet, length, 11, 2);
	index += 5;
	
	index = xfire_add_att_name(local_packet,5, "type");
	local_packet[index] = 0x02;
	local_packet[index+1] = 0x01;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "status");
	local_packet[index] = 0x05;
	local_packet[index+1] = 0x01;
	index += 2;
	
	index = xfire_add_att_name(local_packet,index, "t");
	local_packet[index] = 0x01;
	local_packet[index+1] = (char)strlen(away);
	local_packet[index+2] = 0x00;
	index += 3;
	
	memcpy(local_packet+index,away,strlen(away));
	index += strlen(away);

	return index;
}

/*Send away message to the server*/
int xfire_create_away(char *local_packet, char *away){
	int length = 9+strlen(away);
	int index = 0;
	xfire_add_header(local_packet, length, 32, 1);
	index += 5;
	
	local_packet[index] = 0x2e;
	local_packet[index+1] = 0x01;
	local_packet[index+2] = (char)strlen(away);
	local_packet[index+3] = 0x00;
	index += 4;
	
	memcpy(local_packet+index,away,strlen(away));
	index += strlen(away);

	return index;
}

/*send im back message to the server*/
int xfire_set_back_old(char *local_packet){
	int index = 0;
	xfire_add_header(local_packet, 15, 11, 2);
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "type");
	local_packet[index] = 0x02;
	local_packet[index+1] = 0x00;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index += 5;
	
	return index;
}

/*send im back message to the server*/
int xfire_set_back(char *local_packet){
	int index = 0;
	xfire_add_header(local_packet, 9, 32, 1);
	index += 5;

	local_packet[index] = 0x2e;
	local_packet[index+1] = 0x01;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	index += 4;
	
	return index;
}

/*send keep alive packet to the server*/
int xfire_keep_alive_packet(char *local_packet){
	int index = 0;
	xfire_add_header(local_packet, 26, 13, 2);
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "value");
	local_packet[index] = 0x02;
	local_packet[index+1] = 0x00;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	local_packet[index+4] = 0x00;
	index += 5;
	
	index = xfire_add_att_name(local_packet,index, "stats");
	local_packet[index] = 0x04;
	local_packet[index+1] = 0x02;
	local_packet[index+2] = 0x00;
	local_packet[index+3] = 0x00;
	index += 4;
	
	return index;
}
