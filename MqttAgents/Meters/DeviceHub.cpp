#include "DeviceHub.h"

using namespace std;

bool DeviceHub::Load(string dir) {
   vector<string> dev;
   string dev_dir = Conf::configdir+"/"+dir;;
   string analog_dir = dev_dir+"/AnalogMeter";
   string electric_dir = dev_dir+"/ElectricMeter";
   Device* curdev;
   if(FileManager::isDir(analog_dir)) {
      string a_dir = dir+"/AnalogMeter";
      FileManager::fileList(analog_dir, dev);
      for(auto i=0; i<dev.size(); i++) device.push_back(new AnalogMeter(a_dir, FileManager::getStem(dev[i])));
   }
   dev.clear();
   if(FileManager::isDir(electric_dir)) {
      FileManager::fileList(electric_dir, dev);
      string e_dir = dir+"/ElectricMeter";
      for(auto i=0; i<dev.size(); i++) device.push_back(new ElectricMeter(e_dir, FileManager::getStem(dev[i])));
   }
   for(auto i=0; i<device.size(); i++) {
      device[i]->Connect(this);
      device[i]->Describe();
   }
   current = 0;
   //print();
   return true;
}

bool DeviceHub::Reload(string dir) {
	DynamicPage::Clear();
	this->clear();
	device.clear();
	Load(dir);
}

bool DeviceHub::sync() {
   if(current >= device.size()) current = 0;
   device[current++]->sync();
   return true;
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
