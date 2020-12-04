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
#include "sha.h"

/* converts a hex char array into a dezimal number */
int xfire_hex_to_int(unsigned char hex[], int count)  
{
	int sum = 0;
	int i;
	int temp;
	/* do some black magic conversion stuff */
	for (i = 0; i < count; i++)
	{
		temp = (hex[i] & 0xf0) >> 4;
		sum = sum*16 + temp;
		temp = (hex[i] & 0x0f);
		sum = sum*16 + temp;
	}
	return sum;
}

/*returns the int value of a single hex char*/
int xfire_hex_to_intC(unsigned char hex){
	char temp [1] = {hex};
	return xfire_hex_to_int(temp,1);
}

void hexVal(int val, char *buffer){
	if(val < 0){
		val += 256;
	}
	char *temp = (char*)g_strdup_printf("%02x",val);
	memcpy(buffer,temp,strlen(temp));
	g_free(temp);
}

/* 
 *Does the opposite of toString
 */
void decodeString(char *dest, char *source){
	int length = strlen(source);
	int i;
	
	char temp[] = {0,0,0};
	
	for(i = 0;i < length;i+=2){
		temp[0] = source[i];
		temp[1] = source[i+1];
		dest[i/2] = strtol(temp, NULL, 16); 
	}
}

/*converts a char array to a string {0x00} = "00"*/
void toString(char *dest, const char *source, int length){

	int i;	
	char temp[2];
	
	for(i = 0;i < length;i++){
		hexVal((int)source[i],temp);
		memcpy(dest+(i*2),temp,2);
	}
	
	dest[length*2] = 0x00;
}

/*give string and you will get the hash*/
void hashSha1(char *string, char *sha){

	char sha_local[40];
	int i;	
	int number;
	char hex[2];
	SHA_CTX ctx;

	shaInit(&ctx);
	shaUpdate(&ctx, string, strlen(string));
	shaFinal(&ctx, sha);	
	
	for(i = 0;i < 20;i++){
		number = sha[i];
		if(number < 0){
			number = number + 256;
		}
				
		hexVal(number,hex);
		memcpy(sha_local+(i*2),hex,2);
	}
	memcpy(sha,sha_local,40);
}



