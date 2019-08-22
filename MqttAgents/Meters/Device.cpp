/*
 * Device.cpp
 *
 *  Created on: Jun 23, 2017
 *      Author: Attilio
 */
#include "Device.h"

using namespace std;

bool Device::Connect(std::map<string,vector<Device*>>* hub) {
    string src = "from/"+in+"_event_exec";
    if(hub->count(src)==0) {
	 vector<Device*> tmp;
         tmp.push_back(this);
	 (*hub)[src] = tmp;
    }
    else (*hub)[src].push_back(this);
    return true;
}

bool Device::execCmd(string& topic, string& vv, string& tm) {
    return true;
}

bool Device::execEvent(string& topic, string& vv, string& tm) {
    return true;
}

bool Device::Describe() {
   return true;
}

bool Device::sync() {
  return true;
}

bool Device::sendStatus() {
   return true;
}

bool Device::reset() {
   return true;
}
