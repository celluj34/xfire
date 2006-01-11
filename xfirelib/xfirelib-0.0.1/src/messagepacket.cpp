

#include "messagepacket.h"
#include "xfireparse.h"
#include "variablevalue.h"
#include <iostream>
#include <string>
#include "xdebug.h"

namespace xfirelib {
  using namespace std;

    MessagePacket::MessagePacket(){
        packetID = 133;
    }

int MessagePacket::getPacketContent(char *packet){
    memcpy(packet,buf,bufLength);
    packetID = 2;
    return 150;
}
  void MessagePacket::parseContent(char *buf, int length, int numberOfAtts) {
     
    bufLength = length;
    XINFO(( "Got IM\n" ));
    
    int index = 0;
    sid = new VariableValue();
    peermsg = new VariableValue();
    msgtype = new VariableValue();

    index += sid->readName(buf,index);
    index++; //ignore 03
    index += sid->readValue(buf,index,16);

    index += peermsg->readName(buf,index);
    index++;
    index++;
    index += msgtype->readName(buf,index);
    index++;
    index += msgtype->readValue(buf,index,4);

    if(msgtype->getValue()[0] == 0){
        imindex = new VariableValue();
        index += imindex->readName(buf,index);

        VariableValue messageTemp;
        index++;//ignore 02
        index += imindex->readValue(buf,index,4);
        index += messageTemp.readName(buf,index);
        index++;
        int messageLength = buf[index];
        index++;
        index++;
        index = messageTemp.readValue(buf,index,messageLength);

        for(int i = 0; i < messageTemp.getValueLength();i++){
            message += messageTemp.getValue()[i];
        }
        /*TODO: implement this and answer the package*/
    }else if(msgtype->getValue()[0] == 1){
        cout << "got ack for a message we have sent" << endl;
    }else if(msgtype->getValue()[0] == 2){
       memcpy(this->buf,buf,150);
        /*answer the packet*/
        cout << "some auth magic stuff" << length << endl;
    } 
  }

};
