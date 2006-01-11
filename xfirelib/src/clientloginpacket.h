

#ifndef __CLIENTLOGINPACKET_H
#define __CLIENTLOGINPACKET_H



#include "xfirepacketcontent.h"
#include "variablevalue.h"
#include <string.h>

namespace xfirelib {

   /**
    *The Login Packet
    *This Packet creates the Login Packet with the crypted Password
    */
  class ClientLoginPacket : public XFirePacketContent {
  public:
    XFirePacketContent* newPacket() { return new ClientLoginPacket(); }

    int getPacketId() { return 1; }
    int getPacketContent(char *buf);
    int getPacketAttributeCount();
    int getPacketSize() { return 300; };
    void setUsername(std::string name) {this->name = name;}
    void setPassword(std::string password) {this->password = password; };

    /**
     *Set the salt the server sent us to crypt the password
     *@param salt The VariableValue object that we extracted from the packet
     */
    void setSalt(VariableValue *salt) {this->salt = salt; };

    void parseContent(char *buf, int length, int numberOfAtts) { };
  private:
    void cryptPassword(unsigned char *crypt);
    void hashSha1(const char *string, unsigned char *sha);
    int length;
    std::string name;
    std::string password;
    VariableValue *salt;
  };

};

#endif
