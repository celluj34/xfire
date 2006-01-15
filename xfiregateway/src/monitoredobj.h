
#ifndef __MONITOREDOBJ_H
#define __MONITOREDOBJ_H

#include <typeinfo>
#include <string>
#include <xfirelib/xdebug.h>
#include <map>

namespace xfiregateway {
  class MonitoredObj {
  public:
    MonitoredObj(std::string classname);
    ~MonitoredObj();
    int getTotalObjectCount();
    
    std::string classname;
    static std::map<std::string,int> instances;
  };


};



#endif
