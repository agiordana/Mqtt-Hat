#ifndef _DeviceHub_
#define _DeviceHub_

#include <map>
#include "Device.h"

class DeviceHub: public std::map<std::string, std::vector<Device*>> {
public:
   bool Load(std::string);
   bool Reload(std::string);
   bool sendStatus();
   std::string number() {return hsrv::unsigned2a(device.size());};
   bool print();
private:
   std::vector<Device*> device;
   bool Describe();
};

#endif
