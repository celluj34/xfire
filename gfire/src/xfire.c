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
#include "request.h"

#include "x_utils.h"
#include "x_create.h"
#include "x_search.h"
#include "x_packets.h"
#include "x_games.h"

#define XFIRE_SERVER "cs.xfire.com"
#define XFIRE_PORT 25999
#define XFIRE_VERSION 100

#define XFIRE_CONNECT_STEPS 3

GSList *xfire_connections = NULL;
gboolean xfire_ingame_flag = FALSE;
GaimConnection *xfire_gc = NULL;

struct xfire_data { 
	int fd;
	guint ka_source;
 	guint xqf_source;
	gchar *email;
};

struct xfire_new_buddy {
	GaimConnection *gc;
	char *name;
};

struct xfire_buddy { 
	GaimConnection *gc;
	char *sid;
	char *message;
}; 

struct xfire_game {
	GaimConnection *gc;
	char *sid;
	int game;
	int port;
	char *ip;
	int game2;
};

/* prototype defined later */
 int xfire_check_xqf_cb(GaimConnection *gc);

static void xfire_write_packet(GaimConnection *gc, const char *message, int size)
{
	struct xfire_data *ndata = (struct xfire_data *)gc->proto_data;

	write(ndata->fd, message, size);

}

void xfire_close(GaimConnection *gc)
{
	struct xfire_data *ndata = (struct xfire_data *)gc->proto_data;

	if (gc->inpa)
		gaim_input_remove(gc->inpa);

	if (!ndata)
		return;
	
	close(ndata->fd);
 	/* remove keepalive and xqf timeout sources */
 	g_source_remove(ndata->ka_source);
 	g_source_remove(ndata->xqf_source);
	g_free(ndata->email);
	g_free(ndata);
	xfire_games_cleanup();
	xfire_linfo_cleanup();
	xfire_gc = NULL;
}

/* shows the content of a array, only for debugging */
void show_val(char *packet,int length){
	int i;
	char *buffer = (char *)malloc((length*2+1)*sizeof(char));
	
	toString(buffer,packet,length);
		
	for(i = 0;i < length*2;i += 2){
		gaim_debug(GAIM_DEBUG_MISC, "", "%c%c",buffer[i],buffer[i+1]);
	}
	gaim_debug(GAIM_DEBUG_MISC, "", "\n");
	free(buffer);
}

void xfire_update_buddy(GaimConnection *gc, GaimBuddy *buddy){
	char *amessage = (char *)gaim_blist_node_get_string((GaimBlistNode*)buddy,"amessage");
	if(amessage == NULL){
		serv_got_update(gc, buddy->name, TRUE, 0, 0, 0, 0);
	}else{
		serv_got_update(gc, buddy->name, TRUE, 0, 0, -1, UC_UNAVAILABLE);
	}
}

/* 
 *send im message 
 */
int xfire_send_im(GaimConnection *gc, const char *who, const char *message, GaimConvImFlags flags)
{
	GaimBuddy *buddy;
	GaimAccount *account;
	char sid[16];
	char buffer[1024];
	int imindex;
	int im_size;
	
	char temp[256];
	int index = 0;
	
	account = gaim_connection_get_account(gc);
	buddy = gaim_find_buddy(account, who); 
	if(buddy == NULL){
		gaim_conv_present_error(who, account, "Message could not be sent . Buddy not in contact list");
		return 1;
	}
	if(GAIM_BUDDY_IS_ONLINE(buddy)){
		while(index < strlen(message)){
			decodeString(sid,(char *)gaim_blist_node_get_string((GaimBlistNode*)buddy,"sid"));
			/* Count the messages we sent, XFire wants this*/
			imindex = gaim_blist_node_get_int((GaimBlistNode*)buddy,"imindex");
			imindex++;
			gaim_blist_node_set_int((GaimBlistNode*)buddy,"imindex",imindex);
	
			strncpy(temp, message+index, 255);
			temp[255] = 0x00;
		
			im_size = xfire_make_im(buffer, sid,(char *)temp,imindex);
	
			xfire_write_packet(gc, buffer, im_size);
			gaim_debug(GAIM_DEBUG_MISC, "xfire", "send im part :%d %s\n",index,temp);
			index += 255;
		}
	}else{
		gaim_conv_present_error(who, account, "Message could not be sent . Buddy offline");
	}
	
	return 1; 
}

int xfire_keep_alive(GaimConnection *gc){

	if (gaim_connection_get_state(gc) != GAIM_DISCONNECTED) 
	{
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "send keep_alive packet\n");
		char buffer[50];
		int buffer_size = xfire_keep_alive_packet(buffer);
		xfire_write_packet(gc, buffer, buffer_size);
		return 1;
	}
	
	return 0;
}


/*read the salt from the packet we received, this is for login
 *we should use read_dynamic_attr here...
 */
void xfire_login_salt(char *packet, int length, char *salt)
{	
	int i;
	char salt_temp[41];
	salt_temp[40] = '\0';
	
	/* salt begins after the 7th byte (13th byte, but we removed 5 for the header) */
	for(i = 8;i < length;i++){
		salt_temp[i-8] = packet[i];
	}
	strncpy(salt,salt_temp,41);
}



/*
 *Prepares the auth packet to be sent
 */
int xfire_send_auth(char *local_packet,char *packet, int len, GaimConnection *gc){	
	char *passwd = (char *)gaim_account_get_password(gc->account);
	char *name = (char *)gaim_account_get_username(gc->account);
	char salt[41]; 					/*the salt we got from the server*/
	int index = 0;
	
	xfire_login_salt(packet,len, salt);		/*extract salt from packet*/
	
	index = xfire_create_auth(local_packet,salt,passwd,name);
	
	return index;
}



/* is used to find the user that sent us a IM and show it in his chat window */
void xfire_check_buddy_im(gpointer key, gpointer value, gpointer user_data){
  GaimBuddy *buddy; 
  GaimAccount *account;
  GaimConnection *gc;
  struct xfire_buddy *data; 
  
  buddy = (GaimBuddy*)value;
  data = (struct xfire_buddy*) user_data;
  gc = (GaimConnection*) data->gc;
  
  account = gaim_connection_get_account(gc);    
 
  if(buddy->account == account){
  	if(GAIM_BUDDY_IS_ONLINE(buddy)){
		char temp[16];
		decodeString(temp,(char *)gaim_blist_node_get_string((GaimBlistNode*)buddy,"sid"));
		if(memcmp(temp,data->sid,16) == 0){
			serv_got_im(gc,buddy->name,data->message,0, time(NULL));
		}
	}
  }
}

/* is used to find the user that sent us a IM and show it in his chat window */
void xfire_check_buddy_game(gpointer key, gpointer value, gpointer user_data){
  GaimBuddy *buddy; 
  GaimAccount *account;
  GaimConnection *gc;
  struct xfire_game *data; 
  char *sid;
  
  buddy = (GaimBuddy*)value;
  data = (struct xfire_game*) user_data;
  gc = (GaimConnection*) data->gc;
  
  account = gaim_connection_get_account(gc); 
  if(buddy->account == account){
  	if(GAIM_BUDDY_IS_ONLINE(buddy)){
		char temp[16];
		sid = (char *)gaim_blist_node_get_string((GaimBlistNode*)buddy,"sid");
		if(sid != NULL){
			decodeString(temp,sid);
			if(memcmp(temp,data->sid,16) == 0){
		 
				gaim_debug(GAIM_DEBUG_MISC, "xfire", "%s is playing %d\n",buddy->alias,data->game);
			
				if(data->game == 0){
					gaim_blist_node_set_int((GaimBlistNode*)buddy,"game",0);
					gaim_blist_node_set_int((GaimBlistNode*)buddy,"port",0);
					gaim_blist_node_set_string((GaimBlistNode*)buddy,"ip",NULL);
				}else{
				
					gaim_blist_node_set_int((GaimBlistNode*)buddy,"game",data->game);
					gaim_blist_node_set_int((GaimBlistNode*)buddy,"port",data->port);
					gaim_blist_node_set_string((GaimBlistNode*)buddy,"ip",data->ip);
				}
				xfire_update_buddy(gc,buddy);
			}
		}
	}
  }
}

/* is used to find the user that changed status, and set it*/
void xfire_check_buddy_status(gpointer key, gpointer value, gpointer user_data){
  GaimBuddy *buddy; 
  GaimAccount *account;
  GaimConnection *gc;
  struct xfire_buddy *data; 

  buddy = (GaimBuddy*)value;
  data = (struct xfire_buddy*) user_data;
  gc = (GaimConnection*) data->gc;

  account = gaim_connection_get_account(gc);
	

  if(buddy->account == account){
  	if(GAIM_BUDDY_IS_ONLINE(buddy)){
		char temp[16];
		decodeString(temp,(char *)gaim_blist_node_get_string((GaimBlistNode*)buddy,"sid"));

		if(memcmp(temp,data->sid,16) == 0){
		
			if(strncmp("Online",data->message,strlen(data->message))==0 || strlen(data->message) == 0){
				gaim_debug(GAIM_DEBUG_MISC, "xfire", "user %s is no more away\n",buddy->name,data->message);
				gaim_blist_node_set_string((GaimBlistNode*)buddy,"amessage",NULL);		
			}else{			
				gaim_debug(GAIM_DEBUG_MISC, "xfire", "user %s changed status to %s\n",buddy->name,data->message);
				gaim_blist_node_set_string((GaimBlistNode*)buddy,"amessage",data->message);
			}
			xfire_update_buddy(gc,buddy);
		}
	}
  }
}

/* reads the im and depending of the type of the message, does something 
TODO: make 3 fonctions: ack, auth and read message
*/
int xfire_get_im(char *local_packet, char *packet, int len, GaimConnection *gc){
	char sid[16];
	int index = xfire_read_fix_attr(sid, packet,"sid",len,strlen("sid")+1,0,16);
	char msgtype[4];
	int message_length;
	char *text;
	GaimBuddyList *blist;
	struct xfire_buddy data;
	index = xfire_read_fix_attr(msgtype, packet,"msgtype",len,strlen("msgtype")+1,index,4);	
	
	/* the message is a chat message */
	if(msgtype[0] == 0){
		char *message = findString(packet,"im",len,strlen("im"),2);
	
		/* make response packet */
		xfire_add_header(local_packet, 62, 2, 2);
		memcpy(local_packet+5,packet,58);
		local_packet[35] = 0x02;
		local_packet[45] = 0x01;
		
		/* read the message */
		message_length = xfire_hex_to_intC(message[3]);
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "message length %d",message_length);
		text = (char*)malloc((message_length+1)*sizeof(char));
		show_val(message,message_length);
		memcpy(text,message+5,message_length);
		text[message_length] = 0x00;
		
		blist = gaim_get_blist();
		
		data.gc = gc;
		data.sid = sid;
		data.message = text;
		
		g_hash_table_foreach(blist->buddies, &xfire_check_buddy_im, &data);
		
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "im message = %s\n",text);
		free(text);
		return 62;
	}else{
		if(msgtype[0] == 1){/* the message is a ack for a IM we sent */
			return 0;
		}else{
		if(msgtype[0] == 2){/* packet is a athorisation question.. just respond it, dont know what it realy does*/			
			xfire_add_header(local_packet, 155, 2, 2);
			memcpy(local_packet+5,packet,len);
			return 155;
		}
		}
	}	
	return 0;
}

/* adds a buddy we received in the contact list */
void xfire_add_buddy(char *id, char *nick,GaimConnection *gc,char *name){
	GaimBuddy *buddy;
	GaimAccount *account;
	account = gaim_connection_get_account(gc);
	buddy = gaim_find_buddy(account, id);	
	if (buddy== NULL){
		buddy = gaim_buddy_new(account, id, nick);
		gaim_blist_node_set_string((GaimBlistNode*)buddy,"username",name);
		gaim_blist_add_buddy(buddy,NULL,NULL,NULL);
		gaim_blist_node_set_int((GaimBlistNode*)buddy,"game",0);
		gaim_blist_node_set_int((GaimBlistNode*)buddy,"port",0);
		gaim_blist_node_set_string((GaimBlistNode*)buddy,"ip",NULL);
	}else{
		gaim_blist_node_set_string((GaimBlistNode*)buddy,"amessage",NULL);
		gaim_blist_node_set_int((GaimBlistNode*)buddy,"game",0);
		gaim_blist_node_set_int((GaimBlistNode*)buddy,"port",0);
		gaim_blist_node_set_string((GaimBlistNode*)buddy,"ip",NULL);
		gaim_blist_alias_buddy(buddy,nick);
		/*xfire_update_buddy(gc,buddy);*/
	}
}


/*reads the packet which tells me if a user is online or just went offline*/
void xfire_read_buddy_online(char *packet,int len, GaimConnection *gc){
	int number;
	int index;
	
	char ids[300][4];	
	char sids[300][16];
	char userid[5];
	char stringSid[33];
	
	GaimBuddy *buddy;
	GaimAccount *account;
	
	int i = 0;
	int count = 0;
	int loop = 0;
		
	index = xfire_read_dynamic_attr(ids,packet,len, "userid",&number,4,0);	
	index = xfire_read_dynamic_attr(sids,packet,len, "sid",&number,16,0);
	account = gaim_connection_get_account(gc);
	
	/* Cycle trough each user in this update packet */
	for(;i < number;i++){		
		toString(userid, ids[i], 4);
		
		buddy = gaim_find_buddy(account, userid);	
		if(buddy != NULL){
			 
			count = 0;
			loop = 0;
			/* count the number of 0x00 in the sid, if the whole sid is made of 0x00, the buddy singed off */
			for(;loop < 16;loop++){
				if((char)sids[i][loop] == 0x00){
					count++;
				}
			}
			
			if(count == 16){
				serv_got_update(gc, buddy->name, FALSE, 0, 0, 0, 0);
			}else{
				xfire_update_buddy(gc,buddy); 
			}
			
			toString(stringSid, sids[i], 16);
			gaim_blist_node_set_string((GaimBlistNode*)buddy,"sid",stringSid);
			gaim_blist_node_set_int((GaimBlistNode*)buddy,"imindex",0);
		}
		
	}
}

/* reads all buddys from the buddy list and adds them to the buddylist in gaim */
void xfire_read_buddys(char *packet,unsigned int len, GaimConnection *gc){
	int number;
	char friends[300][100]; //username = 25 long max
	int index = xfire_read_dynamic_var_attr(friends,packet,len, "friends",&number,0,100);/* read the usernames */
	char nicks[300][100]; 
	char ids[300][4];
	int i = 0;
	char userid[5];
	index = xfire_read_dynamic_var_attr(nicks,packet,len, "nick",&number,index,100);/* read the nicknames */
	
	index = xfire_read_dynamic_attr(ids,packet,len, "userid",&number,4,index);/* read the userids */	
	for(;i < number;i++){
		if((char)nicks[i][0] == 0x00){
			memcpy(nicks[i],friends[i],100);/* if there is no name nick set, then use the username */
		}
		/*encode the userid to a string, because we show it on the gui and it looks like {0x23,0x34,0x53,0x00}*/
		
		toString(userid, ids[i], 4); 
		
		xfire_add_buddy(userid,nicks[i],gc,friends[i]);
		
	}
}

void xfire_accept_buddy(struct xfire_new_buddy *buddy){
	char buffer[512];
	int length = 0;
	length = xfire_invitation_accept(buffer,buddy->name);
	
	xfire_write_packet(buddy->gc, buffer, length);
	free(buddy->name);
	free(buddy);
}

void xfire_deny_buddy(struct xfire_new_buddy *buddy){
char buffer[512];
	int length = 0;
	length = xfire_invitation_deny(buffer,buddy->name);
	
	xfire_write_packet(buddy->gc, buffer, length);
	free(buddy->name);
	free(buddy);
}

/*
reads the invitation
*/
void xfire_read_invitation(GaimConnection *gc, char *packet, int len){
	char *name = (char *)malloc(sizeof(char)*26);
	struct xfire_new_buddy *buddy = (struct xfire_new_buddy *)malloc(sizeof(struct xfire_new_buddy));
	
	char *msg;
	
	xfire_read_var_attr(name, packet, "name",len,strlen("name")+3, 0,26);
	gaim_debug(GAIM_DEBUG_MISC, "xfire", "user %s wants to add us in his contact list\n",name);
	
	buddy->name = name;
	buddy->gc = gc;
	
	msg = g_strdup_printf(_("The user %s wants to add %s to his or "
					 "her buddy list."),
				   name, gaim_account_get_username(gc->account));
	
	gaim_request_action(gc, NULL, msg, NULL,
						GAIM_DEFAULT_ACTION_NONE, buddy, 2,
						_("Authorize"), G_CALLBACK(xfire_accept_buddy),
						_("Deny"), G_CALLBACK(xfire_deny_buddy));
} 



/*
stub, prevents sending our whole contact list, because in this protocol,
the userid and the username is not the same. we use the username to add a buddy,
but use the userid to communicate
*/
static void xfire_new_buddies(GaimConnection *gs, GList *buddies, GList *groups){
	
}



/*
packet_length 00 type(12) 00 numberOfAtts
attribute_length 'name' number_of_names name_length 00 name
attribute_length 'fname'  number_of_fnames fname_length 00 fname //first name
attribute_length 'lname'  number_of_lnames lname_length 00 lname //last name
attribute_length 'email'  number_of_emails email_length 00 email
*/
static void xfire_new_buddy(GaimConnection *gc, GaimBuddy *buddy, GaimGroup *group){	
	char *buffer = (char*)malloc(sizeof(char) * 512);
	char name[26];
	int index = 0;
	strncpy(name,buddy->name,strlen(buddy->name)+1);
	index = xfire_add_new_buddy(buffer, name);
	
	xfire_write_packet(gc, buffer, index);
	gaim_blist_remove_buddy(buddy);
	free(buffer);
}

/*remove a buddy*/
void xfire_rem_buddy(GaimConnection *gc, GaimBuddy *buddy, GaimGroup *group)
{
	char buffer[17]; 
	char name[26];
	strncpy(name,buddy->name,strlen(buddy->name)+1);
	
	xfire_delete_buddy(buffer, name);
	xfire_write_packet(gc, buffer, 17);	
}

/*
read the buddy status
*/
void xfire_read_status(GaimConnection *gc, char *packet, int len){	
	char sid[16];
	char type[4];
	char text[200];
	char status[2];
	struct xfire_buddy data;
	GaimBuddyList *blist = gaim_get_blist();
	int index = xfire_read_fix_attr(sid, packet,"sid",len,strlen("sid")+1,0,16);
	
	index = xfire_read_fix_attr(type, packet,"type",len,strlen("type")+1,index,4);	
	
	if(type[0]==0){
		memcpy(text,"Online",sizeof("Online"));
	}else{
		index = xfire_read_fix_attr(status, packet,"status",len,strlen("status"),index,2);
	
		index = xfire_read_var_attr(text, packet, "t",len,1, index,200);
	}
	
	data.gc = gc;
	data.sid = sid;
	data.message = text;
	
	g_hash_table_foreach(blist->buddies, &xfire_check_buddy_status, &data);	
}

/*
read the buddy status
*/
void xfire_read_status_new(GaimConnection *gc, char *packet, int len){	
	char sid[16];
	char text[200];
	struct xfire_buddy data;
	GaimBuddyList *blist = gaim_get_blist();
	int index = xfire_read_fix_attr(sid, packet,"sid",len,strlen("sid")+4,0,16);

	index = xfire_read_var_attr(text, packet, "msg",len,strlen("msg")+3, index,200);

	data.gc = gc;
	data.sid = sid;
	data.message = text;
	
	g_hash_table_foreach(blist->buddies, &xfire_check_buddy_status, &data);
}

/*
read the game status of the buddys
*/
void xfire_game_status(GaimConnection *gc, char *packet, int len){
	unsigned int number_sid = 0;
	unsigned int number_game = 0;
	unsigned int number_gip = 0;
	unsigned int number_gport = 0;
	char sid[300][16];
	char game[300][4];
	char ip[300][4];
	char port[300][4];
	int index = 0;
	int loop = 0;
	int temp_ip1;
	int temp_ip2;
	int temp_ip3;
	int temp_ip4;
	struct xfire_game data;
	
	GaimBuddyList *blist = gaim_get_blist();
	
	index = xfire_read_dynamic_attr(sid,packet,len, "sid",&number_sid,16,index);	
	index = xfire_read_dynamic_attr(game,packet,len, "gameid",&number_game,4,index);
	index = xfire_read_dynamic_attr(ip,packet,len, "gip",&number_gip,4,index);
	index = xfire_read_dynamic_attr(port,packet,len, "gport",&number_gport,4,index);
		
	for(; loop < number_sid;loop++){
		data.gc = gc;
		data.sid = sid[loop];
		data.game = (int)(xfire_hex_to_intC(game[loop][0])+(xfire_hex_to_intC(game[loop][1])*256)); 
		data.game2 = (int)(xfire_hex_to_intC(game[loop][2])+(xfire_hex_to_intC(game[loop][3])*256)); 
		data.port = (int)(xfire_hex_to_intC(port[loop][0])+(xfire_hex_to_intC(port[loop][1])*256)); 

		temp_ip4 = xfire_hex_to_intC(ip[loop][0]);
		temp_ip3 = xfire_hex_to_intC(ip[loop][1]);
		temp_ip2 = xfire_hex_to_intC(ip[loop][2]);
		temp_ip1 = xfire_hex_to_intC(ip[loop][3]);
		data.ip = g_strdup_printf("%d.%d.%d.%d",temp_ip1,temp_ip2,temp_ip3,temp_ip4);
		
		g_hash_table_foreach(blist->buddies, &xfire_check_buddy_game, &data);
	}
	
}

void xfire_login_check(GaimConnection *gc, char *packet, int len){
	int buffer_size = 0;
	char buffer[1024];
	int index = 0;
	char nick[50];
	
	index = xfire_read_var_attr(nick, packet, "nick",len,strlen("nick"), index,50);
	
	if((char *) gaim_account_get_alias(gc->account) != NULL){
	
	if(strcmp(nick,(char *) gaim_account_get_alias(gc->account)) != 0){
		buffer_size = xfire_send_alias_change(buffer,(char *) gaim_account_get_alias(gc->account));
		xfire_write_packet(gc, buffer, buffer_size);
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "we sent our new nick: %s\n",(char *) gaim_account_get_alias(gc->account));
	}
	}
}

/*handles all incoming packets*/
void xfire_packet_handler(gpointer data, gint source, GaimInputCondition condition)
{		
	static int temp_length = 0;
	static int temp_buffer_index = 0;
	unsigned int temp_length_size = 0;
	static char *temp_buffer;
	
	GaimConnection *gc = data;
	
	char header[5];
	unsigned int len;
	int command;
	int atts;
	char *buffer = (char*)malloc(1024*sizeof(char)); /*stores the packet that we send*/
	int buffer_size = 0;
	
	char *packet = (char*)malloc(65536*sizeof(char)); /*stores the packet we just received*/
	int pack_temp_size;
	
	/* read the first 4 bytes as char in the array header */
	int readHeader = 0;
	
	if(temp_length > 0){			/*If we have a packet that isnt completely read*/
		temp_length_size = read(source, (void*)packet, temp_length);
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "ahhhh error: %d and %d, add it at %d\n",temp_length,temp_length_size,temp_buffer_index);
		temp_length = temp_length - temp_length_size;
		memcpy(temp_buffer+temp_buffer_index,packet, temp_length_size);
		temp_buffer_index += temp_length_size;
		if(temp_length == 0){
			memcpy(header,temp_buffer,5);
			memcpy(packet,temp_buffer+5,65531);
			free(temp_buffer);
			temp_buffer_index = 0;
		}else{			
			free(packet);
			free(buffer);
		return;
		}
	}else{							/*normal behavior*/
		readHeader = read(source, (void*)header, 5);	
	
		if ( readHeader != 5) {
			gaim_connection_error(gc, _("Unable to read header from server"));
			xfire_close(gc);
			free(packet);
			free(buffer);
			return;
		}
		/* read the length and the type of the packet from the header */
		len = xfire_hex_to_intC(header[0])+(xfire_hex_to_intC(header[1])*256);
		command = xfire_hex_to_intC(header[2]);
		atts = xfire_hex_to_intC(header[4]);
	
		/* read the rest from the packet, we use the indicated length to do that */
		
		len = len-5;
		
		pack_temp_size = read(source, (void*)packet, len);
		
		if ( pack_temp_size != len && len > 0) {
			gaim_debug(GAIM_DEBUG_MISC, "xfire", "error: read %d but should be %d\n",pack_temp_size,len);
			temp_length = len - pack_temp_size;
			temp_buffer = (char *)malloc(sizeof(char)*65536);
			
			memcpy(temp_buffer,header,5);
			memcpy(temp_buffer+5,packet,pack_temp_size);
			temp_buffer_index += pack_temp_size+ 5;
			free(packet);
			free(buffer);
			return;
		}
	}
	
	/* read the length and the type of the packet from the header */
	len = xfire_hex_to_intC(header[0])+(xfire_hex_to_intC(header[1])*256);
	command = xfire_hex_to_intC(header[2]);
	atts = xfire_hex_to_intC(header[4]);
	
	len = len-5;
	
	/* do a different action for each type of packet */
	switch(command){
	case 128: 
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "received: login salt from server\n");
		buffer_size = xfire_send_auth(buffer,packet, len, gc);		
		xfire_write_packet(gc, buffer, buffer_size);
		gaim_connection_update_progress(gc, "Login sent", 2, XFIRE_CONNECT_STEPS);
		break;
	case 129:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "received: wrong passwd/username\n");
		gaim_connection_error(gc, "wrong passwd/username");
		break;
	case 130:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "Logged\n");
		gaim_connection_set_state(gc, GAIM_CONNECTED);
		/*Set alias*/
		xfire_login_check(gc,packet,len);
		struct xfire_data *ndata = (struct xfire_data *)gc->proto_data;
 		ndata->ka_source = g_timeout_add(300000, (GSourceFunc)xfire_keep_alive, gc);
 		ndata->xqf_source = g_timeout_add(15000, (GSourceFunc)xfire_check_xqf_cb, gc);
		break;
	case 131:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got buddylist: names and nicks\n");
		xfire_read_buddys(packet,len, gc);
		break;
	case 132:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got buddylist: user is online\n");
		xfire_read_buddy_online(packet,len,gc);
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "Correct\n");
		break;
	case 133:		
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got IM\n");
		buffer_size = xfire_get_im(buffer, packet, len, gc);
		if(buffer_size != 0){
			xfire_write_packet(gc, buffer, buffer_size);
		}
		break;
	case 134:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "login ok, but version too old\n");
		gaim_connection_error(gc, "login ok, but version too old, change version number in the account options");
		break;
	case 135:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got buddylist:game that a buddy is playing\n");
		xfire_game_status(gc,packet,len);
		break;
	case 136:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got list of friends of friends\n");
		break;
	case 137:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "invitation result\n");
		/*gaim_notify_error(gc,"XFire Error","Buddy add Error","User unknown");*/
		break;	
	case 138:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got invitation\n");		
		xfire_read_invitation(gc, packet, len);
		break;	
	case 139:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "ack for remove\n");
		break;
	case 141:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got prefs\n");
		break;
	case 142:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got buddylist: status of somebody\n");
 		xfire_read_status(gc, packet, len);
		break;
	case 143:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "Got some buddy information\n");
		break;
	case 144:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got 'did', dont know now what it is\n");		
		break;
	case 145:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "connection closed because of another loggin\n");	
		gaim_connection_error(gc, "Someone else is online with our account");	
		break;
	case 147:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "Buddy is playing? no idea\n");
		break;
	case 148:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "we got some game information?\n");
		break;
	case 154:	
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "got buddylist: status of somebody /new \n");
		show_val(header,5);
		show_val(packet,len);
 		xfire_read_status_new(gc, packet, len);
	default:
		gaim_debug(GAIM_DEBUG_MISC, "xfire", "received: length=%i command=%i atts=%i packet:\n",  len, command, atts);	
		show_val(header,5);
		show_val(packet,len);
	}
	free(packet);
	free(buffer);
	
}

/* 002 - MSG_CLIENT_LOGIN */
void xfire_login_connect(gpointer data, gint source, GaimInputCondition cond)
{
	GaimConnection *gc = data;
	char oc2[1024];
	int length;
	GaimAccount *account = gaim_connection_get_account(gc);
	struct xfire_data *ndata = (struct xfire_data *)gc->proto_data;
		
	if (!g_list_find(gaim_connections_get_all(), gc)) {
		close(source);
		return;
	}

	if (source < 0) {
		gaim_connection_error(gc, _("Unable to connect."));
		return;
	}

	ndata->fd = source;

	/* Update the login progress status display */
	
	gaim_connection_update_progress(gc, "login", 1, XFIRE_CONNECT_STEPS);	

	xfire_write_packet(gc, "UA01", 4); /* open connection */
	
	length = xfire_open_connection2(oc2,gaim_account_get_int(account, "version", XFIRE_VERSION));
	xfire_write_packet(gc, oc2, length);
	
	gc->inpa = gaim_input_add(ndata->fd, GAIM_INPUT_READ, xfire_packet_handler, gc);
}

void xfire_login(GaimAccount *account)
{
	GaimConnection *gc = gaim_account_get_connection(account);
	xfire_gc = gc; /* so we can get access to this from join_game */

	gaim_connection_update_progress(gc, _("Connecting"), 0, XFIRE_CONNECT_STEPS);	
	
	gc->proto_data = g_new0(struct xfire_data, 1);
	if (gaim_proxy_connect(account,
				gaim_account_get_string(account, "server", XFIRE_SERVER),
				gaim_account_get_int(account, "port", XFIRE_PORT),
				xfire_login_connect, gc) != 0) {
		gaim_connection_error(gc, _("Unable to connect."));
	} else {
		/* load game xml from user dir */
		xfire_parse_games(g_build_filename(gaim_user_dir(), "gfire_games.xml", NULL));
	/* load launch game information from user dir */
 		xfire_parse_linfo_xml(g_build_filename(gaim_user_dir(), "gfire_launch.xml", NULL));
	}
}



const char* xfire_list_icon(GaimAccount *a, GaimBuddy *b)
{
	return "xfire";
}

void xfire_list_emblems(GaimBuddy *b, char **se, char **sw, char **nw, char **ne)
{
	char *amessage;
	int game;
	if (b->present == GAIM_BUDDY_OFFLINE){
		*se = "offline";
	}
	else{
		amessage = (char *)gaim_blist_node_get_string((GaimBlistNode*)b,"amessage");
		if(amessage != NULL){
			*se = "away";
		}
		game = gaim_blist_node_get_int((GaimBlistNode*)b,"game");
		if(game != 0){
			*se = "game";
		}
	}
	
	
}


GHashTable *xfire_chat_info_defaults(GaimConnection *gc, const char *chat_name)
{
	GHashTable *defaults;

	defaults = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, g_free);

	if (chat_name != NULL)
		g_hash_table_insert(defaults, "group", g_strdup(chat_name));

	return defaults;
}

GaimPlugin *my_protocol = NULL;

static void
xfire_set_away(GaimConnection *gc, const char *state, const char *msg){	
	char buffer[1024];
	int length;
	
	if (msg != NULL)
	{
		length = xfire_create_away(buffer,msg);
		xfire_write_packet(gc, buffer, length);
	}else{
		if (state){
			if (!strcmp(state, _("Away From Computer"))){
				length =xfire_create_away(buffer,_("Away From Computer"));
				xfire_write_packet(gc, buffer, length);
			}
			else if(!strcmp(state,_("Busy"))){
				length = xfire_join_game(buffer, 4236, 0, NULL);
				xfire_write_packet(gc, buffer, length);
			}else{
				length =xfire_set_back(buffer);
				xfire_write_packet(gc, buffer, length);
			}
		}else{
			length =xfire_set_back(buffer);
			xfire_write_packet(gc, buffer, length);
		}
	} 
}

static GList *
xfire_away_states(GaimConnection *gc)
{
	GList *m = NULL;
	m = g_list_append(m, _("Available"));
	m = g_list_append(m, _("Away From Computer"));
	/*m = g_list_append(m, _("Busy"));*/
	return m;
}


static gchar *xfire_prpl_tooltip_text(GaimBuddy *b)
{
	GString *ret;
	char *amessage;
	int game;
	int port;
	char *ip;
	char *username;
	char game_text[60];
	ret = g_string_new("");
			
	if (GAIM_BUDDY_IS_ONLINE(b)){
		amessage = (char *)gaim_blist_node_get_string((GaimBlistNode*)b,"amessage");
		game = (int)gaim_blist_node_get_int((GaimBlistNode*)b,"game");
		port = (int)gaim_blist_node_get_int((GaimBlistNode*)b,"port");
		username = (char *)gaim_blist_node_get_string((GaimBlistNode*)b,"username");
		
		g_string_append_printf(ret, "\n<b>Username</b>: %s", username);
		
		if (amessage != NULL) {
			g_string_append_printf(ret, "\n<b>%s</b>: %s", _("Away"), amessage);
			
		}
		
		if(game != 0){
			
			xfire_game_name(game_text, game);
			g_string_append_printf(ret, "\n<b>Game</b>: %s", game_text);
			ip = (char *)gaim_blist_node_get_string((GaimBlistNode*)b,"ip");
			if(ip != NULL && strcmp(ip,"0.0.0.0")!=0){
				g_string_append_printf(ret, "\n<b>Game IP</b>: %s", ip);	
			}
			if(port != 0){
				g_string_append_printf(ret, "\n<b>Game Port</b>: %d", port);
			}
			
		}
 	}
	return g_string_free(ret, FALSE);
}

  /**
  *	sees if a buddy is in game
  *
  *	@param b				gaim buddy pointer of buddy to check
  *
  *	@return	boolean		true if buddy is in a game false otherwise
 */
 gboolean xfire_is_buddy_ingame(GaimBuddy *b)
 {
 	int game;
 		
	game = (int)gaim_blist_node_get_int((GaimBlistNode*)b,"game");
 	if(game != 0)
 			return TRUE;
 		else return FALSE;
 }
 
 
 /**
  *  GSource watch pid callback for glib.  This function waits for a join game
  *  PID to exit.  This tells gfire to remove user out of game when the user
  *  quits the game. This function is not called directly but by glib.  The
  *  function prototype is set by glib.  This function does not return anything
  *
  *  @param      pid     Integer PID of the process to watch (wait for to die)
  *  @param      status  status of exiting pid
  *  @param      data    Pointer to data passed in by setup function
  *
 */
 void
 xfire_game_watch_cb(GPid pid, int status, gpointer *data)
 {
 	/* were now out of game, clean up pid send network message */
 
 	char packet[1024];
 	int len = 0;
 
 	g_spawn_close_pid(pid);
 	memset(packet,0x00, 1024);
 	len = xfire_join_game((char *)&packet, 0, 0, NULL);
 	if (len) xfire_write_packet((GaimConnection *)data, packet, len);
 	xfire_ingame_flag = FALSE;
 }


 /**
  * Joins a game in progress with buddy. spawns process, puts us in game with xfire
  * and adds a gsource watch on the child process so when it exits we can take
  * ourselves out of game.
  *
  * @param gc			pointer to our GaimConnection for sending network traffic.
  * @param sip		server ip (quad dotted notation) of where the game is
  * @param sport		server port of where the game is located
  * @param game		the xfire game ID to join (looked up in launch options)
  *
  * asking to join a game that is not playable (not configured through launch options
  * or any other reason for an unplayable game is not defined.  You must check
  * the game playability with xfire_game_playable()
  *
  * @see xfire_game_playable
 */
 void xfire_game_join(GaimConnection *gc, const gchar *sip, int sport, int game)
 {
 	/* test stuff */
 	char packet[1024];
 	int len = 0;
 	xfire_linfo *linfo =NULL;
 	gchar *command = NULL;
 	char **argvp;
 	int argcp = 0;
 	gboolean worked = FALSE;
 	GPid pid;
 	GError *gerr;
 	
 	memset(packet,0x00, 1024);
 	linfo = xfire_linfo_get(game);
 	if (!linfo) {
 		gaim_debug(GAIM_DEBUG_ERROR, "xfire", "Launch info struct not defined!\n");
 		return;
 	}
 	
 	command = xfire_linfo_get_cmd(linfo, sip, sport, NULL);
 	g_shell_parse_argv(command, &argcp, &argvp, &gerr);
 	gaim_debug(GAIM_DEBUG_MISC, "xfire", "Attempting to join game %d, on server %s , at port %d\n", game, sip, sport);
 	gaim_debug(GAIM_DEBUG_MISC, "xfire", "launch xml command parsed to:\n");
 	gaim_debug(GAIM_DEBUG_MISC, "xfire", "%s\n", command);
 	worked = g_spawn_async(linfo->c_wdir, argvp, NULL,
 						(GSpawnFlags)G_SPAWN_DO_NOT_REAP_CHILD,
 						NULL, NULL, &pid, &gerr);
 	if (!worked) {
 		/* something went wrong! */
 		gaim_debug(GAIM_DEBUG_ERROR, "xfire", "Launch failed, message: %s\n", gerr->message);
 		g_free(command);
 		g_strfreev(argvp);
 		return;
 	}
 	
 	/* program seems to be running! */
 	xfire_ingame_flag = TRUE;
 	len = xfire_join_game((char *)&packet, game, sport, sip);
 	if (len) xfire_write_packet(gc, packet, len);
 	/* need to watch pid so we can get out of game when game closes */
 	g_child_watch_add(pid, (GChildWatchFunc)xfire_game_watch_cb, (gpointer *)gc);
 	g_free(command);
 	g_strfreev(argvp);
 
}
 
 
 /**
  * Callback function for gaim buddy list right click menu.  This callback
  * is what the interface calls when the Join Game ... menu option is selected
  *
  * @param node		BuddyList node provided by interface
  * @param data		Gpointer data (not used)
*/
 void xfire_buddy_menu_joingame_cb(GaimBlistNode *node, gpointer *data)
{
 	int game = 0;
	gchar *serverip = NULL;
 	int serverport = 0;
 	
 	game = xfire_get_buddy_game(node);
	if (game && xfire_game_playable(game)) {
 		serverport = xfire_get_buddy_port(node);
 		if (serverport) serverip = (gchar *)xfire_get_buddy_ip(node);
 		
 		xfire_game_join(xfire_gc, serverip, serverport, game);
 	}
 }
 
 
 /**
  *	gaim callback function.  Not used directly.  Gaim calls this callback
  *	when user right clicks on Xfire buddy (but before menu is displayed)
  *	Function adds right click "Join Game . . ." menu option.  If game is
  *	playable (configured through launch.xml), and user is not already in
  *	a game.
  *
  *	@param	node		Gaim buddy list node entry that was right clicked
  *
 *	@return	Glist		list of menu items with callbacks attached (or null)
*/
 GList * xfire_node_menu(GaimBlistNode *node)
{
	GList *ret = NULL;
	GaimBlistNodeAction *me = NULL;
	GaimBuddy *b =(GaimBuddy *)node;

	if (GAIM_BLIST_NODE_IS_BUDDY(node)) {
		if (xfire_ingame_flag) return NULL; /* don't show menu if we are in game */
		/* add entry to menu if the buddy is in game and game is playable by us */
		if (xfire_is_buddy_ingame(b) &&
			xfire_game_playable(xfire_get_buddy_game(node))
		) {
			me = gaim_blist_node_action_new(_("Join Game ..."),
				(void *)xfire_buddy_menu_joingame_cb, NULL);
			if (!me) {
				return NULL;
			}
			ret = g_list_append(ret, me);
		}
 	}
	return ret;

 }

 /**
  *	g_timeout_add callback function to watch for $HOME/.gaim/ingame.tmp
  *	file.  If this file is found we send join_game data to xfire network
  *	if we are in game and then the file is no longer present then take
  *	client out of game.  Only do this when in game status was met using
  *	ingame.tmp file.  The ingame.tmp file is a copied LaunchInfo.txt file
  *	from XQF.  The user must create a script to copy this file and then
  *	remove it once the game is quit.  We match the contents of the XQF
  *	file against our launch.xml.  Find the game and then send the network
  *	message.  This function is not called directy. It is called by glib.
  *
 *	@param		gc		pointer to gaim connection struct for this connection
  *
  *	@return				returns TRUE always, except if gc connection state
  *						is set to disconnected.
 */
 int xfire_check_xqf_cb(GaimConnection *gc){
 
 	static char *filename = NULL;
 	static gboolean found = FALSE;
 	xfire_xqf_linfo *xqfs = NULL;
 	char packet[1024];
	int len = 0;
 	int game = 0;
 
 	if (gaim_connection_get_state(gc) != GAIM_DISCONNECTED) 
 	{
 		gaim_debug(GAIM_DEBUG_MISC, "xfire", "[XQF callback]: checking for xqf file status\n");
 		if (xfire_ingame_flag && !found) return TRUE;
 		if (!filename) {
			filename = g_build_filename(gaim_user_dir(), "ingame.tmp", NULL);
 		}
 		if (g_file_test(filename, G_FILE_TEST_EXISTS)){
 			if (found) return TRUE;
 			/* file was found, but not previously */
 			found = TRUE;
 			xqfs = xfire_linfo_parse_xqf(filename);
 			if (!xqfs) return TRUE;
 			game = xfire_xqf_search(xqfs);
 			if (!game) {
 				gaim_debug(GAIM_DEBUG_WARNING, "xfire", "[XQF callback]: parsed ingame.tmp. Could not find matching game\n");
 				xfire_xqf_linfo_free(xqfs);
 				return TRUE;
 			}
			memset(packet,0x00, 1024);
 			len = xfire_join_game((char *)&packet, game, xqfs->port, xqfs->ip);
 			if (len) xfire_write_packet(gc, packet, len);
 			xfire_ingame_flag = TRUE;
 			gaim_debug(GAIM_DEBUG_MISC, "xfire", "[XQF callback]: Detected game join (%d) at (%s:%d)\n", game, xqfs->ip, xqfs->port );
 			xfire_xqf_linfo_free(xqfs);
 		} else {
 			if (!found) return TRUE;
 			if (xfire_ingame_flag) {
 				gaim_debug(GAIM_DEBUG_MISC, "xfire", "[XQF callback]: Status file removed, sending out of game msg\n");
 				xfire_ingame_flag = FALSE;
 				memset(packet,0x00, 1024);
 				len = xfire_join_game((char *)&packet, 0, 0, NULL);
 				if (len) xfire_write_packet(gc, packet, len);			
 			}
 			found = FALSE;
 		}
 		return TRUE;
 	}
 	gaim_debug(GAIM_DEBUG_ERROR, "xfire", "[XQF callback]: We are still running the timeout but GC state is disconnected?!?!?\n");
 	return FALSE;
 }
 

static GaimPluginProtocolInfo prpl_info =
{
	OPT_PROTO_CHAT_TOPIC,
	NULL,					/* user_splits */
	NULL,					/* protocol_options */
	NO_BUDDY_ICONS,				/* icon_spec */
	xfire_list_icon,			/* list_icon */
	xfire_list_emblems,			/* list_emblems */
	NULL,					/* status_text */
	xfire_prpl_tooltip_text,		/* tooltip_text */
	xfire_away_states,			/* away_states */
	xfire_node_menu,				/* blist_node_menu */
	NULL,					/* chat_info */
	NULL, 					/* chat_info_defaults */
	xfire_login,				/* login */
	xfire_close,				/* close */
	xfire_send_im,				/* send_im */
	NULL,					/* set_info */
	NULL,					/* send_typing */
	NULL,					/* get_info */
	xfire_set_away,				/* set_away */
	NULL,					/* set_idle */
	NULL,					/* change_passwd */
	xfire_new_buddy,			/* add_buddy */
	xfire_new_buddies,			/* add_buddies */
	xfire_rem_buddy,			/* remove_buddy */
	NULL,					/* remove_buddies */
	NULL,					/* add_permit */
	NULL,					/* add_deny */
	NULL,					/* rem_permit */
	NULL,					/* rem_deny */
	NULL,					/* set_permit_deny */
	NULL,					/* warn */
	NULL,					/* join_chat */
	NULL,					/* reject chat invite */
	NULL,					/* get_chat_name */
	NULL,					/* chat_invite */
	NULL,					/* chat_leave */
	NULL,					/* chat_whisper */
	NULL,					/* chat_send */
	NULL,					/* keepalive */
	NULL,					/* register_user */
	NULL,					/* get_cb_info */
	NULL,					/* get_cb_away */
	NULL,					/* alias_buddy */
	NULL,					/* group_buddy */
	NULL,					/* rename_group */
	NULL,					/* buddy_free */
	NULL,					/* convo_closed */
	NULL,					/* normalize */
	NULL,					/* set_buddy_icon */
	NULL,					/* remove_group */
	NULL,					/* get_cb_real_name */
	NULL,					/* set_chat_topic */
	NULL,					/* find_blist_chat */
	NULL,					/* roomlist_get_list */
	NULL,					/* roomlist_cancel */
	NULL,					/* roomlist_expand_category */
	NULL,					/* can_receive_file */
	NULL					/* send_file */
};

static GaimPluginInfo info =
{
	GAIM_PLUGIN_MAGIC,
	GAIM_MAJOR_VERSION,
	GAIM_MINOR_VERSION,
	GAIM_PLUGIN_PROTOCOL,                             /**< type           */
	NULL,                                             /**< ui_requirement */
	0,                                                /**< flags          */
	NULL,                                             /**< dependencies   */
	GAIM_PRIORITY_DEFAULT,                            /**< priority       */

	"prpl-xfire",                                   /**< id             */
	"Xfire",                              	          /**< name           */
	VERSION,                                          /**< version        */
	                                                  /**  summary        */
	N_("Xfire Protocol Plugin"),
	                                                  /**  description    */
	N_("Xfire Protocol Plugin"),
	NULL,                                             /**< author         */
	GAIM_WEBSITE,                                     /**< homepage       */

	NULL,                                             /**< load           */
	NULL,                                             /**< unload         */
	NULL,                                             /**< destroy        */

	NULL,                                             /**< ui_info        */
	&prpl_info,                                       /**< extra_info     */
	NULL,
	NULL
};

static void init_plugin(GaimPlugin *plugin)
{	
	GaimAccountOption *option;

	option = gaim_account_option_string_new(_("Server"), "server",XFIRE_SERVER);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,option);

	option = gaim_account_option_int_new(_("Port"), "port", XFIRE_PORT);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,option);
	
	option = gaim_account_option_int_new(_("Version"), "version", XFIRE_VERSION);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options,option);

	my_protocol = plugin;
}

GAIM_INIT_PLUGIN(xfire, init_plugin, info);
