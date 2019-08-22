#include "AgentTaskLocal.h"


using namespace std;

DeviceHub AgentTaskLocal::device;

void AgentTaskLocal::EventProcessor(std::string) {
  MsgQueue<Message>* eventsource = Conf::mbx.Attach("evprocessor");
  while(true) {
	  Message m = eventsource->receive(); 
	  Logger::log("EventExecutor", "Event Arrived");
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
	    key = msg.theme+"_event_execEvent";
	    if(device.count(key) >0) {
	       for(auto i=0; i<device[key].size(); i++) device[key][i]->execEvent(msg.theme, value, msg.tm);
	    }
	    key = msg.theme+"_event_sync";
	    if(device.count(key) >0) {
	       for(auto i=0; i<device[key].size(); i++) device[key][i]->sync(msg.theme,value, msg.tm);
	    }
         }
         else if(todo=="status") {
	    key = msg.theme+"_status_sync";
	    if(device.count(key) >0) {
	       for(auto i=0; i<device[key].size(); i++) device[key][i]->sync(msg.theme, value, msg.tm);
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
   boost::property_tree::ptree pt, input, output;
   read_json(ss, pt);
   string dname = pt.get<string>("name");
   string dtype = pt.get<string>("type");
   input = pt.get_child("input");
   output = pt.get_child("output");
   mkManifest(dname, message);
   ifstream devtemplate;
   ifstream controltemplate;
   ifstream componenttemplate;
   ofstream device;
   ofstream control;
   ofstream component;
   string devicefile;
   string description;
   char ch;
   string templatepath;
   if(dtype != "honoff") templatepath = Conf::configdir+"/templates/device.xml";
      else templatepath = Conf::configdir+"/templates/hdevice.xml";
   string devicepath = Conf::configdir+"/devices";
   devicepath = devicepath + "/"+dname;
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
   while(hsrv::strReplace(description, "$MQTT_DOMAIN", Conf::mqtt_domain));
   while(hsrv::strReplace(description, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain));
   while(hsrv::strReplace(description, "$MQTT_SERVICE", Conf::mqtt_service));
   string cmd = "to/"+Conf::mqtt_domain+"/"+Conf::mqtt_subdomain+"/"+Conf::mqtt_service+"/"+dname;
   while(hsrv::strReplace(description, "$CMD", cmd));
   if(dtype == "honoff") {
       string out = "";
       BOOST_FOREACH(boost::property_tree::ptree::value_type &v, output) {
          out = "to/" + v.second.get<string>("name");
       }
       while(hsrv::strReplace(description, "$TARGET", out));
   }

   for(auto i=0; i<description.size(); i++) device.put(description[i]);
   device.close();

   //create the component description
   unsigned j = 0;
   if(dtype == "honoff") {
     BOOST_FOREACH(boost::property_tree::ptree::value_type &v, input) {
        string out = v.second.get<string>("name");
        string name = v.second.get<string>("alias");
        mkComponent(name, devicepath, out, j);
        j++;
     }
   }
   else {
     BOOST_FOREACH(boost::property_tree::ptree::value_type &v, output) {
        string out = v.second.get<string>("name");
        string name = v.second.get<string>("alias");
        mkComponent(name, devicepath, out, j);
        j++;
     }
   }
   //
   //create control
   string trigger = "";
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, input) {
      if(trigger != "") trigger += "+";
      trigger += "from/";
      trigger += v.second.get<string>("name");
   }
   mkControl(dname, dtype, devicepath, trigger, j);

   return true;
}

bool AgentTaskLocal::Delete(string& message) {
   stringstream ss;
   ss<<message;
   boost::property_tree::ptree pt;
   read_json(ss, pt);
   string dname = pt.get<string>("name");
   string fname = dname +".json";
   string manifestpath = Conf::configdir+"/devicemanifest";
   string devpath = Conf::configdir+"/devices/"+dname;
   if(!FileManager::isDir(devpath)) return false;
   FileManager::deleteFile(manifestpath, fname);
   string cmd = "/bin/rm -r "+devpath;
   hsrv::cmdExec(cmd);
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

bool AgentTaskLocal::mkComponent(string& name, string& destination, string& output, unsigned order) {
   string templatepath = Conf::configdir+"/templates/component.xml";
   string description="";
   char ch;
   string destfile = destination+"/"+name+".xml";
   ifstream componenttemplate;
   ofstream component;
   //
   //load template
   componenttemplate.open(templatepath.c_str());
   if(!componenttemplate.is_open()) return false;
   while((ch=componenttemplate.get())&&!componenttemplate.eof()) description+=ch;
   componenttemplate.close();
   componenttemplate.clear();
   //
   //create the component
   component.open(destfile.c_str());
   if(!component.is_open()) return false;
   while(hsrv::strReplace(description,"$NAME",name));
   string event = "from/"+output;
   string cmd = "to/"+output;
   while(hsrv::strReplace(description,"$EVENT",event));
   while(hsrv::strReplace(description,"$CMD",cmd));
   hsrv::strReplace(description,"$ORDER", hsrv::unsigned2a(order));
   for(auto j=0; j<description.size(); j++) component.put(description[j]);
   component.close();
   component.clear();
   return true;
}

bool AgentTaskLocal::mkControl(string& name, string& dtype, string& path, string& trig, unsigned sz) {
   string templatepath; 
   string controlfile = path + "/control.xml";
   if(dtype == "updown") templatepath = Conf::configdir+"/templates/controlupdown.xml";
   else if(dtype == "honoff") templatepath = Conf::configdir+"/templates/hcontrol.xml";
   else templatepath = Conf::configdir+"/templates/control.xml";
   ifstream controltemplate;
   ofstream control;
   char ch;
   string description = "";
   
   //load template
   controltemplate.open(templatepath.c_str());
   if(!controltemplate.is_open()) return false;
   while((ch=controltemplate.get())&&!controltemplate.eof()) description+=ch;
   controltemplate.close();
   //
   //create control header
   string cmd = "to/"+Conf::mqtt_domain+"/"+Conf::mqtt_subdomain+"/"+Conf::mqtt_service+"/"+name;
   while(hsrv::strReplace(description, "$CMD", cmd));
   while(hsrv::strReplace(description, "$TRIGGER", trig));
   //create trigger input
   if(dtype == "updown") description =  mkAutomatonUpDown(description, trig);
   else {
     description = mkTrigger(description, trig);
     // create control automaton
     description = mkAutomaton(description, dtype, sz);
   }
   // save control file
   control.open(controlfile.c_str());
   for(size_t i=0; i<description.size(); i++) control.put(description[i]);
   control.close();
   return true;
}

string AgentTaskLocal::mkTrigger(string& ctempl, string& trig) {
   string input = "";
   string description = ctempl;
   NameList events;
   events.init(trig,'+');
   for(size_t i=0; i<events.size(); i++) {
	   input += "<attribute name=\"";
	   input += events[i];
	   input += "\" category=\"input\" value=\"1\" />\n";
   }
   hsrv::strReplace(description, "$INPUT", input);
   string remap = "";
   for(size_t i=0; i<events.size(); i++) {
	   remap += "<attribute name=\"";
	   remap += events[i];
	   remap += "\" category=\"remap\" value=\"ON\" />\n";
   }
   hsrv::strReplace(description, "$REMAP", remap);
   return description;
}


string AgentTaskLocal::mkAutomatonUpDown(string& ctempl, string& trig) {
   string res = ctempl;
   NameList events;
   events.init(trig, '+');
   if(events.size() != 2) return res;
   while(hsrv::strReplace(res, "$TUP", events[0]));
   while(hsrv::strReplace(res, "$TDOWN", events[1]));
   return res;
}

string AgentTaskLocal::mkAutomaton(string& ctempl, string& devtype, unsigned n) {
   string res = ctempl;
   string current="";
   string next="";
   string offoff="";
   string control = "";
   vector<string> gray;

   generateGrayCode((int)n, gray);

   if(devtype == "onoff") {
      for(unsigned i=0; i<n; i++) {
         if(i>0) next += "+";
         next += "ON";
         if(i>0) offoff += "+";
         offoff += "OFF";
      }
      for(size_t i=0; i<gray.size(); i++) {
         current = gray[i];
         if(control != "") control += '\n';
         control += "<attribute name=\"";
         control += current;
         control += "\" category=\"control\" value=\"";
         if(current!=next) control += next;
	 else control += offoff;
         control += "\"/>";
      }
   }
   else if(devtype == "cyclic") {
      for(size_t i=0; i<gray.size(); i++) {
         current = gray[i];
         next = (i<gray.size()-1 ? gray[i+1] : gray[0]);
         if(control != "") control += '\n';
         control += "<attribute name=\"";
         control += current;
         control += "\" category=\"control\" value=\"";
         control += next;
         control += "\"/>";
      }
   }
   hsrv::strReplace(res, "$CONTROL", control);
   return res;
}

bool AgentTaskLocal::generateGrayCode(int N, vector<string>& codes) {

   N = pow(2,N);
   string grey[N];
   int i,j,n=2;
   grey[0] ="OFF";
   grey[1] ="ON";
   while(n<N) {
      n = n*2;
      for(i=n/2,j=n/2-1;i<n && j>=0;i++,j--)
      grey[i] = grey[j];
      for(i=0;i<n/2;i++)
      grey[i] = "OFF+" + grey[i];
      for(i=n/2;i<n;i++)
      grey[i] = "ON+" + grey[i];
   }
   for(i=0;i<n;i++) {
      codes.push_back(grey[i]);
   }
   return true;
}

string AgentTaskLocal::cleanString(string& s) {
   string tmp="";
   for(auto i=0; i<s.length(); i++)
	   if(s[i]!='"' && s[i] != '{' && s[i] != '}' &&s[i] != ' ') tmp+=s[i];
   return tmp;
}
