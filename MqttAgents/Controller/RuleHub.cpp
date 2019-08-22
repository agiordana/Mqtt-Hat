#include "RuleHub.h"

using namespace std;

bool RuleHub::Load(string dir) {
   vector<string> tmp;
   string rl_dir = Conf::configdir+"/"+dir;
   if(!FileManager::isDir(rl_dir)) return false;
   FileManager::fileList(rl_dir, tmp);
   for(auto i=0; i<tmp.size(); i++) {
      Rule* r = new Rule(this, dir, FileManager::getStem(tmp[i]));
      rule.push_back(r);
   }
   for(auto i=0; i<condition.size(); i++) {
      condition[i]->connect(this);
   }
   for(auto i=0; i<event.size(); i++) {
      event[i]->connect(this);
   }
   //print();
   for(auto i=0; i<rule.size(); i++) rule[i]->Describe();
   return true;
}

bool RuleHub::Reload(string dir) {
	DynamicPage::Clear();
	this->clear();
	condition.clear();
	event.clear();
	rule.clear();
        return Load(dir);
}

bool RuleHub::print() {
   cout<< "---------------- Rules ------------"<<endl;
   for(auto i=0; i<rule.size(); i++) rule[i]->print();
   cout<< "---------------- Conditions ------------"<<endl;
   for(auto i=0; i<condition.size(); i++) condition[i]->print();
   cout<< "---------------- Events ------------"<<endl;
   for(auto i=0; i<event.size(); i++) event[i]->print();
   cout<< "----------------- Table -----------------"<<endl;
   map<string,vector<Input*>>::iterator it;
   for(it=begin(); it!=end(); it++) {
	   cout<<"topic: "<<it->first<<" --- ";
	   for(auto i=0; i<it->second.size(); i++) cout<<(it->second)[i]->name<<" ";
	   cout<<endl;
   }
   return true;
}

bool RuleHub::timeUpdate() {
   for(auto i=0; i<rule.size(); i++) rule[i]->update();
   return true;
}
