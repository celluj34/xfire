
#include "xdebug.h"
#include "client.h"
#include "clientinformationpacket.h"
#include "clientversionpacket.h"
#include "xfirepacket.h"
#include "authpacket.h"
#include "clientloginpacket.h"
#include "buddylistnamespacket.h"
#include "messagepacket.h"
#include "sendmessagepacket.h"
#include "messageackpacket.h"
#include "sendkeepalivepacket.cpp"
#include <iostream>

//#define XFIRE_HOST "cs.xfire.com"
#define XFIRE_HOST "204.71.190.24"
#define XFIRE_PORT 25999

//#define UINT_8 unsigned char
//#define UINT_32 unsigned long

namespace xfirelib {

using namespace std; 

Client::Client() {
  XDEBUG(("Client constructor ...\n"));
  packetReader = new PacketReader(NULL);
  packetReader->addPacketListener( this );
  buddyList = new BuddyList( this );
}

  void Client::connect( string username, string password ) {
    try {
      this->username = new string(username);
      this->password = new string(password);
      socket = new Socket( XFIRE_HOST, XFIRE_PORT );
      packetReader->setSocket(socket);
      startThreads();
      //packetReader->startListening();
      
      
      socket->send("UA01");
      XDEBUG(("Sent UA01\n"));
      ClientInformationPacket *infoPacket = new ClientInformationPacket();
      XFirePacket *packet = new XFirePacket(infoPacket);
      packet->sendPacket( socket );
      XINFO(("sent ClientInformationPacket\n"));

      ClientVersionPacket *versionPacket = new ClientVersionPacket();
      versionPacket->setProtocolVersion( 50 );
      packet = new XFirePacket(versionPacket);
      packet->sendPacket( socket );
      XINFO(("sent ClientVersionPacket\n"));
      
      delete packet;
      delete infoPacket;
      delete versionPacket;
    } catch( SocketException ex ) {
      XERROR(("Socket Exception ?! %s \n",ex.description().c_str() ));
    }
  }
  void Client::startThreads() {
    void* (*func)(void*) = &xfirelib::Client::startReadThread;
    XINFO(("About to start thread\n"));
    int ret = pthread_create( &readthread, NULL, func, (void*)this );
    void* (*func2)(void*) = &xfirelib::Client::startSendPingThread;
    int ret2 = pthread_create( &sendpingthread, NULL, func2, (void*)this );
  }
  void *Client::startReadThread(void *ptr) {
    ((Client*)ptr)->packetReader->run();
  }
  void *Client::startSendPingThread(void *ptr) {
    Client *me = (Client*)ptr;
    while(1) {
      sleep(60 * 3); // Sleep for 3 minutes
      XDEBUG(( "Sending KeepAlivePacket\n" ));
      SendKeepAlivePacket packet;
      me->send( &packet );
    }
  }

  void Client::disconnect(){
    delete socket;
  }

  void Client::send( XFirePacketContent *content ) {
    XFirePacket *packet = new XFirePacket(content);
    packet->sendPacket( socket );
    delete packet;
  }

  void Client::addPacketListener( PacketListener *listener ) {
    packetReader->addPacketListener( listener );
  }


  void Client::receivedPacket( XFirePacket *packet ) {
    XDEBUG(("Client::receivedPacket\n"));
    if( packet == NULL ) {
      XERROR(("packet is NULL !!!\n"));
      return;
    }
    if( packet->getContent() == NULL ) {
      XERROR(("ERRRR getContent() returns null ?!\n"));
      return;
    }
    XFirePacketContent *content = packet->getContent();

    switch( content->getPacketId() ) {
    case XFIRE_PACKET_AUTH_ID: {
      XINFO(("Got Auth Packet .. Sending Login\n"));
      AuthPacket *authPacket = (AuthPacket*)packet->getContent();

      ClientLoginPacket *login = new ClientLoginPacket();
      login->setSalt( authPacket->getSalt() );
      login->setUsername( *username );
      login->setPassword( *password );
      send( login );
      delete login;
      break;
    }
    case XFIRE_MESSAGE_ID: {
      XDEBUG(( "Got Message, sending ACK\n" ));
        MessagePacket *message = (MessagePacket*)packet->getContent();
        if(message->getMessageType() == 0){
            MessageACKPacket *ack = new MessageACKPacket();
            memcpy(ack->sid,message->getSid(),16);
            ack->imindex = message->getImIndex();
            send( ack );
	    delete ack;
        }else if(message->getMessageType() == 2){
            send(message);
        }
	break;
    }

    default:
      cout << "Nothing here ... " << endl;
      break;
    }

  }
};
