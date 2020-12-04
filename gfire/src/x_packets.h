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

int xfire_make_im(char *local_packet, char *sid, char *message, int imindex); 
int xfire_open_connection2(char *packet, int version);
int xfire_invitation_accept(char *local_packet, char *name);
int xfire_invitation_deny(char *local_packet, char *name);
int xfire_add_new_buddy(char *buffer, char *name);
int xfire_delete_buddy(char *buffer, char *name); 
int xfire_create_auth(char *local_packet,char *salt,char *passwd,char *name);
int xfire_join_game(char *local_packet, int game, int port, const char *ip);
int xfire_send_alias_change(char *local_packet, char *name);
int xfire_create_away(char *local_packet, char *away);
int xfire_set_back(char *local_packet);
int xfire_keep_alive_packet(char *local_packet);
