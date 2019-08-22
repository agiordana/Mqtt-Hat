#include "Params.h"
#include "NameList.h"
#include "FileManager.h"
#include "Conf.h"
#define CONFIG "/home/debian/CONFIG"
#define HOME "/home/debian/hat_home"
#define HOMEBU "/home/debian/hat_home"
#define BIN "/home/debian/bin"
using namespace std;

int main() {
   string conf = CONFIG;
   string home = HOME;
   string homebu = HOMEBU;
   string cmd;
   NameList filelist;
   NameList cmdlist;
   Params init;
   setof<Attribute>::iterator it;
   setof<Attribute> initprg;
   string initpath = conf + "/bbgpio/init.xml";
   sleep(10);
   if(FileManager::isDir(home)) {
      cmd = "rm -f " + home + "/*/.lock";
      hsrv::cmdExec(cmd);
   }
   if(FileManager::isDir(homebu)) {
      cmd = "rm -f " + homebu + "/*/.lock";
      hsrv::cmdExec(cmd);
   }
   FileManager::dirList(conf, filelist);
   for(size_t i=0; i<filelist.size(); i++) {
      Params global;
      string path = conf + "/" + filelist[i] + "/conf.xml";
      global.xmlLoad(path);
      string cmd = string(BIN) + "/" + global.get("agenttype") + " " + conf + "/" + filelist[i] + "&";
      cmdlist.push_back(cmd);
   }
   init.xmlLoad(initpath);
   initprg = init.extractCategory("script");
   for(it=initprg.begin(); it!=initprg.end(); it++) {
      cout<< it->value<<endl;
      hsrv::cmdExec(it->value);
      sleep(1);
   }
   for(size_t i=0; i<cmdlist.size(); i++) {
      cout<<cmdlist[i]<<endl;
      system(cmdlist[i].c_str());
      sleep(1);
   }
   return 0;
 }
