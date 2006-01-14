#include "packetreader.h"

#include <iostream>
#include <pthread.h>
#include "xfirepacket.h"

#include "loginpacket.h"
#include "clientinformationpacket.h"
#include "authpacket.h"
#include "loginfailedpacket.h"
#include "loginsuccesspacket.h"
#include "buddylistonlinepacket.h"
#include "buddylistnamespacket.h"
#include "buddylistgamespacket.h"
#include "messagepacket.h"
#include "otherloginpacket.h"
#include "invitebuddypacket.h"
#include "inviterequestpacket.h"
#include "recvremovebuddypacket.h"
#include "recvstatusmessagepacket.h"

#include "xdebug.h"
//#include "packetlistener.h"

namespace xfirelib {
  using namespace std;

  PacketReader::PacketReader(Socket *socket) {
    this->socket = socket;
    this->packetListeners = new vector<PacketListener *>();

    initPackets();
  }
  void PacketReader::setSocket(Socket *socket) {
    this->socket = socket;
  }
  PacketReader::~PacketReader() {
    // TODO: delete each packetListener ..
    delete packetListeners;

    delete socket;
    while(!packets->empty()) { delete packets->at(packets->size()-1); packets->pop_back(); }
    delete packets;
  }

  void PacketReader::initPackets() {
    packets = new vector <XFirePacketContent *>();
    packets->push_back( new LoginPacket() );
    packets->push_back( new ClientInformationPacket() );
    packets->push_back( new AuthPacket() );
    packets->push_back( new LoginFailedPacket() );
    packets->push_back( new LoginSuccessPacket() );
    packets->push_back( new MessagePacket() );
    packets->push_back( new BuddyListOnlinePacket() );
    packets->push_back( new BuddyListNamesPacket() );
    packets->push_back( new BuddyListGamesPacket() );
    packets->push_back( new OtherLoginPacket() );
    packets->push_back( new InviteBuddyPacket() );
    packets->push_back( new InviteRequestPacket() );
    packets->push_back( new RecvRemoveBuddyPacket() );
    packets->push_back( new RecvStatusMessagePacket() );
  }


  void *muh(void *ptr);

  /* I moved thread starting to Client

  void PacketReader::startListening() {
    PacketReader *myself = this;
    void* (*func)(void*) = &xfirelib::PacketReader::thread_start;
    XINFO(("About to start thread\n"));
    int ret = pthread_create( &readthread, NULL, func, (void*)myself );
    XDEBUG(("ret: %d\n",ret));
  }
  */

  void PacketReader::run() {
    // start receiving on socket...
    XDEBUG(("Starting run() method .... \n"));
    while(1) {
      string str;
      //int b = socket->recv(str);
      XFirePacket *packet = new XFirePacket(this);
      XDEBUG(("Waiting for next packet ... \n"));
      packet->recvPacket( socket );
      XINFO(("Received packet\n"));
      if(packet->getContent() != NULL) {
	fireListeners( packet );
      } else {
	XDEBUG(("Packet Content was NULL ... Unknown Packet Id ??\n"));
      }
      XDEBUG(("Notified Listeners\n"));// << b << "bytes: " << str << endl;
    }
  }

  void PacketReader::fireListeners( XFirePacket *packet ) {
    for(vector<PacketListener *>::iterator it = packetListeners->begin() ;
	it != packetListeners->end() ; ++it) {
      (*it)->receivedPacket( packet );
    }
  }

  XFirePacketContent *PacketReader::getPacketContentClass(int packetId) {
    XDEBUG(("Searching for a content class...\n"));
    for(int i = 0 ; i < packets->size() ; i++)
      if(packets->at(i)->getPacketId() == packetId) return packets->at(i);
    XDEBUG(("None Found\n"));
    return NULL;
  }

  void PacketReader::addPacketListener( PacketListener *listener ) {
    packetListeners->push_back( listener );
  }
};
