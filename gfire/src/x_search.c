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

/*
 * finds a array in another array, same as findString2, but this is a bad bad hack
 * GO AWAY BAD HACK
 */
 
#include "internal.h"
#include "x_utils.h"

void *findString(const void *s, char *c, int size_s, int size_c, int number_to_find)
{
    const unsigned char *src = s;
    unsigned char *uc = c;
    int count = 0;    
    while (size_s-- != 0) {
        if (memcmp(src,uc,size_c)==0){
		count++;
		if(count == number_to_find){/* ugly hack, because i want to find 'im' in the chat package, not imindex */
            return (void *) src;
	    }
	}
        src++;
    }
    return NULL;
}

/*finds a array in another array
 */
int findString2(const void *s, char *c, int size_s, int size_c, int index){
    const unsigned char *src = s;
    unsigned char *uc = c;
    src = src+index;
    while (size_s-- != index) {
        if (memcmp(src,uc,size_c)==0){
		return ((void *) src)-s;
	    }
	    src++;
	}   
    return -1;
}

/* works for attributes with a dynamic length like:
 * attribute_length 'salt' saltLength_length saltLength 00 salt
 */
int xfire_read_var_attr(char *value, char *packet, char *attr,int packet_length,int attr_length, int start,int max_length){
	int length_index = findString2(packet,attr, packet_length,strlen(attr),start)+attr_length+1;

	unsigned int length = xfire_hex_to_intC(packet[length_index]);

	if(max_length >= length){
		memcpy(value,packet+length_index+2,length);
	}
	if(max_length > length){
		memset(value+length,0,1);
	}
	return length_index+2+length;
}

/* works for attributes with a fix length like:
 * attribute_length 'sid' 03 SID
 */
int xfire_read_fix_attr(char *value, char *packet, char *attr,int packet_length,int attr_length, int start,int attr_size){
	int att_index = findString2(packet,attr, packet_length,strlen(attr),start)+attr_length;	
	memcpy(value,packet+att_index,attr_size);
	return att_index+attr_size;
}

/* reads a attribute like this one: 
 * attribute_length 'userid' 04 02 02 00 userID userID 
 * it stores them in a 2d array
 */
int xfire_read_dynamic_attr(char **values,char *packet,int packet_length, char *attr,int *return_number,int att_len,int start){
	int number_index = findString2(packet,attr, packet_length,strlen(attr),start)+strlen(attr)+2;
	if(number_index == -1){
		return -1;
	}
	unsigned int number = xfire_hex_to_intC(packet[number_index]);
	*return_number = number;
	int i = 0;
	int index = number_index+2;
	char vals[number][att_len];
	
	for(;i < number;i++){
		memcpy(vals[i],packet+index,att_len);
		index +=att_len;
	}
	memcpy(values,vals,att_len*number);
	return index;
}


/* reads a attribute like this one: 
 * attribute_length 'friends' 04 01 numberOfFriends 00 userLength 00 userName userLength 00 userName 
 * it stores them in a 2d array
 */
int xfire_read_dynamic_var_attr(char **values,char *packet,unsigned int packet_length, char *attr,int *return_number,unsigned int start,unsigned int value_len){
	int number_index = findString2(packet,attr, packet_length,strlen(attr),start)+strlen(attr)+2;
	if(number_index == -1){
		return -1;
	}
	int number = xfire_hex_to_intC(packet[number_index]);
	*return_number = number;
	int i = 0;
	int index = number_index+2;
	char vals[number][value_len];
	unsigned int length = 0;
	printf("found %d entries",number);
	
	for(;i < number;i++){
		length = xfire_hex_to_intC(packet[index]);
		index +=2;
		memcpy(vals[i],packet+index,length);
		index +=length;
		if(length >= value_len){
			printf("Error reading at %d of %s, was %d, but %d expected\n",i,attr,length, value_len);
		}
		vals[i][length] = 0x00;
		
	}
	memcpy(values,vals,value_len*number);
	return index;
}

