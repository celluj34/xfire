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

 
void *findString(const void *s, char *c, int size_s, int size_c, int number_to_find);
int findString2(const void *s, char *c, int size_s, int size_c, int index);
int xfire_read_var_attr(char *value, char *packet, char *attr,int packet_length,int attr_length, int start,int max_length);
int xfire_read_fix_attr(char *value, char *packet, char *attr,int packet_length,int attr_length, int start,int attr_size);
int xfire_read_dynamic_attr(char **values,char *packet,int packet_length, char *attr,int *return_number,int att_len,int start);
int xfire_read_dynamic_var_attr(char **values,char *packet,int packet_length, char *attr,int *return_number,int start,int value_len);
