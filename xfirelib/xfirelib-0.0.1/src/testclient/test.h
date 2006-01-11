#ifndef __TEST_H
#define __TEST_H

#include <string>
#include <vector>
#include "../packetlistener.h"
#include "../xfirepacket.h"
#include "../client.h"
#include "../inviterequestpacket.h"

namespace xfirelibtest {
  using namespace std;
  using namespace xfirelib;

  class XFireTestClient : public PacketListener {

  public:
    XFireTestClient(string username, string password);
    ~XFireTestClient();
    void run();

    void receivedPacket(XFirePacket *packet);

  private:
    vector<string> explodeString(string s, string e);
    string joinString(vector<string> s, int startindex, int endindex=-1, string delimiter=" ");
    void printBuddyList();

    Client *client;
    string *lastInviteRequest;

    string username;
    string password;
  };
};


#endif
