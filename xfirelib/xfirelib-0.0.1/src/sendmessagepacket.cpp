#include "sendmessagepacket.h"
#include "xfireutils.h"
#include <string.h>
#include <iostream>
#include "xdebug.h"

namespace xfirelib {
  using namespace std;

  void SendMessagePacket::init(Client *client, string username, string message) {
    BuddyListEntry *entry = client->getBuddyList()->getBuddyByName(username);
    if(entry) {
      setSid(entry->sid);
    }
    this->message = message;
  }
  void SendMessagePacket::setSid(const char *sid) {
    memcpy(this->sid,sid,16);
  }

  int SendMessagePacket::getPacketContent(char *buf) {
    int index = 0;
    VariableValue val;
    val.setName("sid");
    val.setValue(sid,16);

    index += val.writeName(buf,index);
    buf[index++] = 3;
    index += val.writeValue(buf,index);

    val.setName("peermsg");
    index += val.writeName(buf,index);
    buf[index++] = 5;
    //buf[index++] = 7;
    buf[index++] = 3;

    val.setName("msgtype");
    val.setValueFromLong(0,4);
    index += val.writeName(buf,index);
    buf[index++] = 2;
    index += val.writeValue(buf,index);

    val.setName("imindex");
    val.setValueFromLong(imindex,4);
    index += val.writeName(buf,index);
    buf[index++] = 02;
    index += val.writeValue(buf,index);

    val.setName("im");
    val.setValue((char*)message.c_str(),message.size());
    index += val.writeName(buf,index);
    buf[index++] = 01;//TODO: warning, bug with messages over 255 length
    buf[index++] = message.size();
    buf[index++] = 00;
    index += val.writeValue(buf,index);

    return index;
  }
}
