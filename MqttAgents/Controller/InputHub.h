#ifndef _InputHub_
#define _InputHub_

#include <map>
#include "Precondition.h"

class InputHub: public std::map<std::string, std::vector<Input*>> {
public:
   bool Load(std::string);
   bool Reload(std::string);
   bool sendStatus();
   std::string number() {return hsrv::unsigned2a(device.size());};
   bool print();
private:
   std::vector<Input*> device;
   bool Describe();
};

#endif
