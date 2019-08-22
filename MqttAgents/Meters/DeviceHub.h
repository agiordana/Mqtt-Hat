#ifndef _DeviceHub_
#define _DeviceHub_

#include <map>
#include "Device.h"
#include "ElectricMeter.h"
#include "AnalogMeter.h"

class DeviceHub: public std::map<std::string, std::vector<Device*>> {
public:
   bool Load(std::string);
   bool Reload(std::string);
   bool sendStatus();
   bool sync();
   std::string number() {return hsrv::unsigned2a(device.size());};
   bool print();
private:
   size_t current;
   std::vector<Device*> device;
   bool Describe();
};

#endif
