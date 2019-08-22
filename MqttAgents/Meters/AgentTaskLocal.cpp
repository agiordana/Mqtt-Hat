#include "AgentTaskLocal.h"


using namespace std;

DeviceHub AgentTaskLocal::device;

void AgentTaskLocal::EventProcessor(std::string) {
  MsgQueue<Message>* eventsource = Conf::mbx.Attach("evprocessor");
  while(true) {
	  Message m = eventsource->receive(); 
	 // Logger::log("EventExecutor", "Event Arrived");
  }
}

void AgentTaskLocal::CmdExecutor(string id) {
  Logger::log("CmdExecutorLocal", "Started");
  device.Load("devices");
  Logger::log("DeviceNumber", device.number());
  MsgQueue<Message>* cmdsource = Conf::mbx.Attach("cmdexecutor");
  while(true) {
       Message msg = cmdsource->receive();
       string name;
       string value;
       string todo;
       string key;
       string lastfield = FileManager::getTail(msg.theme);
       if(lastfield=="newdevice") {
	       Configure(msg.content);
	       device.Reload("devices");
       }
       else if(lastfield=="devdelete") {
	       Delete(msg.content);
	       device.Reload("devices");
       }
       else if(lastfield!="description"&&lastfield!="logger"){
         decode(msg, todo, name, value);
         if(todo == "request") {
	       if(value=="status") device.sendStatus();
	       if(value=="sync") device.sync();
	       else if(value=="description"||value=="description.json") sendDescription("json");
	       else if(value=="description.xml") sendDescription("xml");
         }
         else if(todo=="cmd") {
            /* execute the command on the hub */
	  key = msg.theme+"_cmd_execCmd";
	  if(device.count(key) >0) {
	     for(auto i=0; i<device[key].size(); i++) device[key][i]->execCmd(msg.theme, value, msg.tm);
	    }
         }
         else if(todo=="event") {
	    key = msg.theme+"_event_exec";
	    if(device.count(key) >0) {
	       for(auto i=0; i<device[key].size(); i++) device[key][i]->execEvent(msg.theme,value, msg.tm);
	    }
         }
    }
  }
}

bool AgentTaskLocal::decode(Message& msg, string& todo, string& name, string& value) {
   NameList todo_value;
   string clean_value = cleanString(msg.content);
   todo_value.init(clean_value, ':');
   if(todo_value.size()!=2) {
      Logger::log("\"MessageDecode_Error\"", msg.content);
      return false;
   }
   name = FileManager::getTail(msg.theme);
   todo = todo_value[0];
   value = todo_value[1]; 
   return true;
}

bool AgentTaskLocal::sendDescription(string language) {
	Logger::describe("description",DynamicPage::Index("",language));
	return true;
}

bool AgentTaskLocal::Configure(string& message) {
   stringstream ss;
   ss<<message;
   boost::property_tree::ptree pt;
   read_json(ss, pt);
   string dname = pt.get<string>("name");
   string dtype = pt.get<string>("type");
   string input = pt.get<string>("input");
   string unit = pt.get<string>("unit");
   string scale_factor = pt.get<string>("scale_factor");
   string probe = pt.get<string>("probe");
   mkManifest(dname, message);
   ifstream devtemplate;
   ofstream device;
   string devicefile;
   string description;
   char ch;
   string templatepath = Conf::configdir+"/templates/"+dtype+".xml";
   string devicepath = Conf::configdir+"/devices/"+dtype;
   FileManager::makeDir(devicepath,true);
   devicefile = devicepath + "/"+dname + ".xml";
   // loads the device template
   devtemplate.open(templatepath.c_str());
   if(!devtemplate.is_open()) return false;
   while((ch=devtemplate.get())&&!devtemplate.eof()) description+=ch;
   devtemplate.close();
   
   // create the device main description;
   device.open(devicefile.c_str());
   if(!device.is_open()) return false;
   while(hsrv::strReplace(description, "$NAME", dname));
   while(hsrv::strReplace(description, "$TYPE", dtype));
   while(hsrv::strReplace(description, "$INPUT", input));
   while(hsrv::strReplace(description, "$UNIT", unit));
   while(hsrv::strReplace(description, "$SCALE_FACTOR", scale_factor));
   while(hsrv::strReplace(description, "$PROBE", probe));
   while(hsrv::strReplace(description, "$MQTT_DOMAIN", Conf::mqtt_domain));
   while(hsrv::strReplace(description, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain));
   while(hsrv::strReplace(description, "$MQTT_SERVICE", Conf::mqtt_service));
   for(auto i=0; i<description.size(); i++) device.put(description[i]);
   device.close();
   return true;
}

bool AgentTaskLocal::Delete(string& message) {
   stringstream ss;
   cout<<message<<endl;
   ss<<message;
   boost::property_tree::ptree pt;
   read_json(ss, pt);
   string dname = pt.get<string>("name");
   string dtype = pt.get<string>("type");
   string fname = dname +".json";
   string xfname = dname +".xml";
   string manifestpath = Conf::configdir+"/devicemanifest";
   string devpath = Conf::configdir+"/devices/"+dtype;
   if(!FileManager::isDir(devpath)) return false;
   FileManager::deleteFile(manifestpath, fname);
   FileManager::deleteFile(devpath, xfname);
   return true;
}

bool AgentTaskLocal::mkManifest(string& dname, string& message) {
   ofstream manifest;
   string manifestpath = Conf::configdir+"/devicemanifest/";
   if(!FileManager::isDir(manifestpath)) FileManager::makeDir(manifestpath, true);
   manifestpath += dname;
   manifestpath += ".json";
   manifest.open(manifestpath.c_str());
   manifest<< message<<endl;
   manifest.close();
   return true;
}

string AgentTaskLocal::cleanString(string& s) {
   string tmp="";
   for(auto i=0; i<s.length(); i++)
            if(s[i]!='"' && s[i] != '{' && s[i] != '}' &&s[i] != ' ') tmp+=s[i];
   return tmp;
}
