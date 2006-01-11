#include "clientloginpacket.h"
#include "xfireutils.h"
#include "SHA1.h"
#include <string.h>
#include <iostream>

namespace xfirelib {
  using namespace std;

  int ClientLoginPacket::getPacketContent(char *packet) {
	int index = 0;
 	/*Username*/

	index = XFireUtils::addAttributName(packet,index, "name");/*add username attribute*/
	packet[index] = 0x01;
	packet[index+1] = (char)name.length();
	packet[index+2] = 0x00;
	index +=3;
	std::copy(name.begin(),name.end(),packet+index);
	index += name.size();

	/*Crypted Password*/
	unsigned char pass[41];
	pass[40] = 0x00;
	cryptPassword(pass);
	index = XFireUtils::addAttributName(packet,index, "password");/*add username attribute*/
	packet[index] = 0x01;
	packet[index+1] = (char)40;
	packet[index+2] = 0x00;
	index += 3;

	memcpy(packet+index,pass,40);
	index += 40;

	length = index;
	return index;
  }

  int ClientLoginPacket::getPacketAttributeCount() {
    return 2;
  }

  void ClientLoginPacket::cryptPassword(unsigned char *crypt){
    std::string total;
    char temp[81];
    CSHA1 sha1;

    total = name+password+"UltimateArena";
    hashSha1(total.c_str(),crypt);
    memcpy(temp,crypt,40);
    memcpy(temp+40,salt->getValue(),40);
    temp[80] = 0x00;

    hashSha1(temp,crypt);
  }



/*give string and you will get the hash*/
void ClientLoginPacket::hashSha1(const char *string, unsigned char *sha){

	char result[41];
        unsigned char temp[1024];
	int i;
	int number;
	char hex[2];
        CSHA1 sha1;
        sha1.Reset();
        sha1.Update((UINT_8 *)string, strlen(string));
        sha1.Final();
        sha1.GetHash(temp);	

        result[0] = 0; 
	char szTemp[16]; 
	for(int i = 0 ; i < 20 ; i++) { 
	  sprintf(szTemp, "%02x", temp[i]); 
	  strcat(result,szTemp); 
	}
	memcpy(sha,result,40);
	sha[40] = 0x00;
}
}
