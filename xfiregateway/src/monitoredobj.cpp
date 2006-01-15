
#include "monitoredobj.h"


namespace xfiregateway {
  std::map<std::string,int> MonitoredObj::instances;// = std::map<std::string,int>();
  MonitoredObj::MonitoredObj(std::string classname) {
    this->classname = classname;
    if(instances.count( classname ) < 1) {
      instances[classname] = 1;
    } else {
      instances[classname]++;
    }
    XDEBUG(( "Constructing new MonitoredObj class: %s - %d\n", classname.c_str(), instances[classname] ));
  }

  MonitoredObj::~MonitoredObj() {
    instances[classname]--;
    XDEBUG(( "DeConstructing MonitoredObj class: %s - %d instances left (total objs: %d)\n", classname.c_str(), instances[classname], getTotalObjectCount() ));
  }

  int MonitoredObj::getTotalObjectCount() {
    std::map<std::string,int>::iterator it = instances.begin();
    int total = 0;
    while(it != instances.end()) {
      total += it->second;
      it++;
    }
    return total;
  }

};
