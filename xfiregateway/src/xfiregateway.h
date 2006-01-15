#ifndef __XFIREGATEWAY_H
#define __XFIREGATEWAY_H

#include <string>
#include <gloox/component.h>
#include <vector>
#include "user.h"
#include "parsegamexml.h"

namespace xfiregateway {
  using namespace std;

  struct User;

  class XFireGateway {
  public:
    XFireGateway(std::map<std::string,std::string> &args);
    ~XFireGateway();
    void run();
    std::string getFQDN();

    gloox::Component *getComponent() { return comp; }

    void writeUserFile();
    void readUserFile();

    User *getUserByJID(std::string barejid);
    void addUser(User *user);
    void removeUser(User *user);
    void quit();
    std::vector<User*> *getUsers() { return users; }

    xfiregateway::ParseGameXML *gameXML;
  private:
    std::map<std::string,std::string> args;
    std::vector<std::string> explodeString(std::string s,
					   std::string e);

    gloox::Component *comp;

    std::vector<User*> *users;
  };

};


#endif

