#ifndef __MESSAGEPACKET_H
#define __MESSAGEPACKET_H

#include "xfirepacketcontent.h"
#include "variablevalue.h"
#include <string>

#define XFIRE_MESSAGE_ID 133

namespace xfirelib {

  class MessagePacket : public XFirePacketContent {
  public:

    MessagePacket();
    XFirePacketContent* newPacket() { return new MessagePacket(); }

    int getPacketContent(char *packet);
    int getPacketId() { return packetID; }
    int getPacketSize() { return 1024; };
    void parseContent(char *buf, int length, int numberOfAtts);
    std::string getMessage() {return message; }
    int getMessageType(){return msgtype->getValue()[0];}
    int getImIndex(){ return imindex->getValue()[0];}//TODO: fix this if we have more than 255 messages
    int getPacketAttributeCount(){ return 2; }
    char * getSid(){ return sid->getValue(); }

    private:
    VariableValue *sid;
    VariableValue *peermsg;
    VariableValue *msgtype;
    VariableValue *imindex;
    std::string message;
    char buf[150];
    int bufLength;
    int packetID;//Special case because we have to answer this packet with id 2
  };
};


#endif
