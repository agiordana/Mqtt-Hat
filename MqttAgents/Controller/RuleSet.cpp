#include "RuleSet.h"

using namespace std;

//__________ Class RuleSet _______________
//

bool RuleSet::Load(string dir) {
   vector<string> rl;
   sring rl_dir = Conf::configdir+"/"+dir;
   Rule* currule;
   if(!FileManager::isDir(dev_dir)) return false;
      FileManager::dirList(rl_dir, rl);
   for(auto i=0; i<rl.size(); i++) {
       config_dir = dir+"/"+rl[i];
       rule.push_back(new Rule(config_dir, rl[i]));
   }
   for(auto i=0; i<rule.size(); i++) rule[i]->Describe();
   print();
   return true;
}

bool RuleSet::Reload(string dir) {
   DynamicPage::Clear();
   this->clear();
   rule.clear();
   return Load(dir);
}

bool RuleSet::connect(InputHub& cond, InputHub event) {
   return true;
}

bool RuleSet::print() {
   map<string,vector<Rule*>>::iterator it;
   for(it=begin(); it!=end(); it++) cout<<it->first<<": "<<it->second.size()<<endl;
   return true;
}
