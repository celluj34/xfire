
#include "xfireparse.h"
namespace xfirelib {

using namespace std;

XFireParse::XFireParse() {
}


/*void XFireParse::readVariableAttribut( char *value, char *packet, char *attr,int packet_length,int attr_length, int start,int max_length ) {
int length_index = findString2(packet,attr, packet_length,strlen(attr),start)+attr_length+1;

	unsigned int length = xfire_hex_to_intC(packet[length_index]);

	if(max_length >= length){
		memcpy(value,packet+length_index+2,length);
	}
	if(max_length > length){
		memset(value+length,0,1);
	}
	return length_index+2+length;
}*/


};
