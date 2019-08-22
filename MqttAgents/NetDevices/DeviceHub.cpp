#include "DeviceHub.h"

using namespace std;

bool DeviceHub::Load(string dir) {
   vector<string> dev;
   string dev_dir = Conf::configdir+"/"+dir;;
   string config_dir;
   Device* curdev;
   if(!FileManager::isDir(dev_dir)) return false;
   FileManager::dirList(dev_dir, dev);
   for(auto i=0; i<dev.size(); i++) {
      config_dir = dir+"/"+dev[i];
      device.push_back(new Device(config_dir, dev[i]));
   }
   for(auto i=0; i<device.size(); i++) {
      device[i]->Connect(this);
      device[i]->Describe();
   }
 //  print();
   return true;
}

bool DeviceHub::Reload(string dir) {
	DynamicPage::Clear();
	this->clear();
	device.clear();
	Load(dir);
}

bool DeviceHub::sendStatus() {
   for(auto i=0; i<device.size(); i++) device[i]->sendStatus();
   return true;
}

bool DeviceHub::print() {
   map<string,vector<Device*>>::iterator it;
   for(it=begin(); it!=end(); it++) cout<<it->first<<": "<<it->second.size()<<endl;
   return true;
}
