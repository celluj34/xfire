#ifndef __CLIENT_H
#define __CLIENT_H

#include <string>
#include <pthread.h>
#include "packetreader.h"
#include "xfirepacketcontent.h"
#include "packetlistener.h"
#include "buddylist.h"

namespace xfirelib {
  struct BuddyList;

class Client : public PacketListener {
 public:
  Client();
  void connect(std::string username, std::string password);
  void send(XFirePacketContent *content);

  BuddyList *getBuddyList() { return buddyList; }
  void addPacketListener(PacketListener *packetListener);
  void disconnect();
  void sendMessage(string username, string message);
  void sendNickChange(string nick);
 protected:
  void receivedPacket( XFirePacket *packet );
  void startThreads();
  static void *startReadThread(void *ptr);
  static void *startSendPingThread(void *ptr);
 private:
  PacketReader *packetReader;
  std::string *username;
  std::string *password;
  Socket *socket;
  BuddyList *buddyList;
  pthread_t readthread;
  pthread_t sendpingthread;
};

};

#endif
