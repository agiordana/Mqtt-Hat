#include "InputHub.h"

using namespace std;

bool InputHub::Load(string dir) {
   vector<string> dev;
   string dev_dir = Conf::configdir+"/"+dir;;
   string config_dir;
   Input* curdev;
   if(!FileManager::isDir(dev_dir)) return false;
   FileManager::dirList(dev_dir, dev);
   for(auto i=0; i<dev.size(); i++) {
      config_dir = dir+"/"+dev[i];
      device.push_back(new Input(config_dir, dev[i]));
   }
   for(auto i=0; i<device.size(); i++) {
      device[i]->Connect(this);
      device[i]->Describe();
   }
 //  print();
   return true;
}

bool InputHub::Reload(string dir) {
	DynamicPage::Clear();
	this->clear();
	device.clear();
        return Load(dir);
}

bool InputHub::sendStatus() {
   for(auto i=0; i<device.size(); i++) device[i]->sendStatus();
   return true;
}

bool InputHub::print() {
   map<string,vector<Input*>>::iterator it;
   for(it=begin(); it!=end(); it++) cout<<it->first<<": "<<it->second.size()<<endl;
   return true;
}
