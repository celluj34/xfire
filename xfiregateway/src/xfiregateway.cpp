/*
 *  xfiregateway - Jabber Gateway for XFire.
 *  Copyright (C) 2006 by
 *          Herbert Poul <herbert.poul@gmail.com> / http://goim.us
 *    http://xfirelib.sphene.net/
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <signal.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <string>
#include <gloox/component.h>
#include <gloox/disco.h>

#include "xfiregateway.h"
#include "gatewayhandler.h"
#include <xfirelib/xdebug.h>
#include <xfirelib/client.h>
#include <string>
#include <map>
#include <time.h>
#include "parsegamexml.h"

#include "simplelib.h"

namespace xfiregateway {
  using namespace gloox;

  XFireGateway::XFireGateway(std::map<std::string,std::string> &args) {
    this->users = 0;
    this->args = args;
    this->gameXML = new xfiregateway::ParseGameXML();
    if(time(&this->startTime) < 0) {
      XERROR(( "time() didn't return a valid value.\n" ));
    }
  }
  XFireGateway::~XFireGateway() {
    XINFO(( "Destructing XFireGateway\n" ));
    writeUserFile();
    std::vector<User*>::iterator it = users->begin();
    while(it != users->end()) {
      delete (*it); //->quit();
      it++;
    }
    users->clear();
    comp->disconnect();


    XINFO(( "Done Deconstructor.\n" ));
  }
  void XFireGateway::addUser( User *user ) {
    users->push_back( user );
    writeUserFile();
  }
  void XFireGateway::removeUser( User *user ) {
    bool removed = false;
    user->preRemove();
    for(std::vector<User*>::iterator it = users->begin() ; it != users->end() ; it++) {
      if((*it) == user) {
	users->erase(it);
	removed = true;
	break;
      }
    }
    if(!removed) {
      XERROR(( "Could not erase user ?? %s\n", user->jid.c_str() ));
    }
    delete user;
    writeUserFile();
  }
  void XFireGateway::readUserFile() {
    users = new vector<User *>;
    struct stat st;
    if(stat("xfiregateway_users.cfg", &st) != 0) {
      XINFO(( "No user file yet ?\n" ));
      return;
    }
    fstream f("xfiregateway_users.cfg", ios_base::in);
    while(!f.eof()) {
      std::string str;
      std::getline(f,str);
      std::vector<std::string> args = explodeString(str," ");
      if(args.size() < 3) continue;
      User *user = new User(this);
      users->push_back( user );
      std::string lowerjid = SimpleLib::stringToLower(args.at(0));
      user->jid = lowerjid;
      user->name = args.at(1);
      user->password = args.at(2);
      XDEBUG(( "User: %s (pass: %s)\n", user->name.c_str(),
	       user->password.c_str() ));
      for(int i = args.size()-1 ; i > 2 ; i--) {
	user->inroster.push_back(args.at(i));
	XDEBUG(( " --- Roster: %s\n", args.at(i).c_str() ));
      }
    }
  }
  std::vector<std::string> XFireGateway::explodeString(std::string s, 
						       std::string e) {
    std::vector<std::string> ret;
    int iPos = s.find(e, 0);
    int iPit = e.length();
    while(iPos > -1) {
      if(iPos != 0)
        ret.push_back(s.substr(0,iPos));
      s.erase(0,iPos+iPit);
      iPos = s.find(e,0);
    }
    if(s!="")
      ret.push_back(s);
    return ret;
  }
  void XFireGateway::writeUserFile() {
    XDEBUG(( "writeUserFile()\n" ));
    fstream f("xfiregateway_users.cfg", ios_base::out | ios_base::trunc);
    for(int i = users->size()-1 ; i >= 0 ; i--) {
      User *user = users->at(i);
      f << user->jid << " " << user->name << " " << user->password;
      for(int j = user->inroster.size()-1 ; j >= 0 ; j--) {
	f << " " << user->inroster.at(j);
      }
      f << endl;
    }
    XDEBUG(( "done writeUserFile()\n" ));
  }

  std::string XFireGateway::getFQDN() {
    return args["component"];
  }
  User *XFireGateway::getUserByJID(std::string barejid) {
    barejid = SimpleLib::stringToLower(barejid);
    std::vector<User*>::iterator it = users->begin();
    while(it != users->end()) {
      if((*it)->jid == barejid)
	return *it;
      it++;
    }
    return NULL;
  }
  User *XFireGateway::getUserByXFireName(std::string user) {
    for( std::vector<User*>::iterator it = users->begin();
	 it != users->end(); it++ ) {
      if((*it)->name == user)
	return *it;
    }
    return NULL;
  }
  void XFireGateway::run() {
    XINFO(( "Running XFireGateway ...\n" ));
    /*
    comp = new gloox::Component( "jabber:component:accept",
				 "localhost",
				 "xfire.localhost",
				 "secret",
				 8888 );
    */
    comp = new gloox::Component( args["namespace"],
				 args["server"],
				 args["component"],
				 args["password"],
				 atoi(args["port"].c_str()));
    Disco *disco = comp->disco();
    disco->addFeature( "jabber:iq:register" );
    disco->addFeature( "jabber:iq:gateway" );
    disco->setIdentity( "gateway", "xfire" );
    disco->setVersion( "GOIM XFire Gateway (C) By Herbert Poul 2006 http://goim.us","pre-pre-alpha", "who cares" );


    GatewayHandler *handler = new GatewayHandler( this );
    comp->registerIqHandler( handler, "jabber:iq:register" );
    comp->registerIqHandler( handler, "jabber:iq:gateway" );
    comp->registerPresenceHandler( handler );
    comp->registerMessageHandler( handler);
    comp->registerConnectionListener( handler );
    comp->registerSubscriptionHandler( handler );

    readUserFile();


    comp->connect();
    XDEBUG(( "Connected ???\n" ));
    //comp->recv( 5 );
    XDEBUG(( " ??? \n" ));
  }
  void XFireGateway::quit() {
  }
};


xfiregateway::XFireGateway *gateway;
pthread_t mainthread;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

  void catchSignal(int signum) {
    static bool alreadyshuttingdown = false;
    XDEBUG(( "Catched Signal ... %d (SIGINT: %d) vs. %ld\n", signum, SIGINT, mainthread ));
    //if(mainthread == pthread_self()) { //!alreadyshuttingdown) {
    pthread_mutex_lock( &mutex1 );
    if(!alreadyshuttingdown) {
      XDEBUG(( "Shutting down...\n" ));
      alreadyshuttingdown = true;
      //signal(SIGINT, catchSignal);
      gateway->quit();
      delete gateway;
      XDEBUG(( "Deleted gateway ... now exiting ..\n" ));
    } else {
      XDEBUG(( "Already shutted down ??\n" ));
      pthread_mutex_unlock( &mutex1 );
      pthread_exit(NULL);
    }
    pthread_mutex_unlock( &mutex1 );
    pthread_exit(NULL);
      /*
    } else {
      //XDEBUG(( "Ignoring .. already shutting down (joining thread)\n" ));
      XDEBUG(( "Joining mainthread\n" ));
      //pthread_join( shuttingdownthread, NULL );
      pthread_join( mainthread, NULL );
      XDEBUG(( "Thread died ...\n" ));
    }
      */
  }

  void printHelp() {
    printf("GOIM XFire Gateway Copyright 2006 By Herbert Poul\n");
    printf("  http://goim.us - http://xfirelib.sphene.net\n\n");
    printf("You need to specify at least one parameter. Valid paramters:\n");
    printf(" --namespace jabber:component:accept (default) or jabber:component:connect\n");
    printf(" --server The server to connect to.\n");
    printf(" --component The component's FQDN (e.g. xfire.myjabberserver.com)\n");
    printf(" --password The component's password\n");
    printf(" --port Port to connect to (default: 5347)\n");
  }
  int main(int argc, char **argv) {
    std::map<std::string,std::string> args;
    args["namespace"] = "jabber:component:accept";
    args["server"] = "localhost";
    args["component"] = "xfire.localhost";
    args["password"] = "secret";
    args["port"] = "5347";
    if(argc < 2) {
      printHelp();
      return 0;
    }
    for(int i = 1 ; i < argc ; i++) {
      std::string param = std::string(argv[i]);
      if(argv[i][0] == argv[i][1] && argv[i][1] == '-') {
	if(param == "--help") {
	  printHelp();
	  return 0;
	} else {
	  args[param.substr(2)] = std::string(argv[++i]);
          printf("Setting %s to %s\n", param.substr(2).c_str(), args[param.substr(2)].c_str() );
	}
      } else {
	printf("WTF ?! %s %c==%d==%d\n", argv[i],argv[i][0],argv[i][1],'-');
      }
    }
    //xfiregateway::XFireGateway gateway;
    mainthread = pthread_self();
    gateway = new xfiregateway::XFireGateway(args);
    signal(SIGINT, catchSignal);
    gateway->run();
  }
  


