/*
 * Device.cpp
 *
 *  Created on: Jun 23, 2017
 *      Author: Attilio
 */
#include "Device.h"

using namespace std;

Device::Device(string dir, string nm): Params::Params(dir, nm) {
        name = nm;
	Params::load();
	load(dir, nm);
	scount = 0;
	turn = 0;
	cycle = 1;
}

bool Device::Connect(std::map<string,vector<Device*>>* hub) {
	vector<string> obs;
	for(size_t i=0; i<component.size(); i++) component[i].getObservations(obs);
	for(auto i=0; i<obs.size(); i++)
		if(obs[i]!="") {
			if(hub->count(obs[i])==0) {
				vector<Device*> tmp;
				tmp.push_back(this);
				(*hub)[obs[i]] = tmp;
			}
			else (*hub)[obs[i]].push_back(this);
		}
	obs.clear();
	control->getObservations(obs);
	for(auto i=0; i<obs.size(); i++)
		if(obs[i]!="") {
			if(hub->count(obs[i])==0) {
				vector<Device*> tmp;
				tmp.push_back(this);
				(*hub)[obs[i]] = tmp;
			}
			else (*hub)[obs[i]].push_back(this);
		}
	obs.clear();
	getObservations(obs);
	for(auto i=0; i<obs.size(); i++)
		if(obs[i]!="") {
			if(hub->count(obs[i])==0) {
				vector<Device*> tmp;
				tmp.push_back(this);
				(*hub)[obs[i]] = tmp;
			}
			else (*hub)[obs[i]].push_back(this);
		}
	obs.clear();
	return true;
}

bool Device::execCmd(string& topic, string& vv, string& tm) {
    string res;
    NameList action;
    string nextstatus;
    string value = aggregateValue(vv);
    action = control->actions(topic,"cmd", get("type"), value, status());
    for(size_t i=0; i<action.size(); i++) {
       if(get("type") == "honoff") res = setValue(action[i]);
       else res = component[i].setValue(action[i]);
    }
    return true;
}

bool Device::execEvent(string& topic, string& vv, string& tm) {
    string res;
    string nextstatus;
    NameList action = control->actions(topic,"event", get("type"), vv, status());
    for(size_t i=0; i<action.size(); i++) {
       res = component[i].setValue(action[i]);
    }
    return true;
}

string Device::aggregateValue(string value) {
   string res = "";
   for(size_t i=0; i<value.length(); i++)
     if(value[i]==' ') res+='+';
	else res+=value[i];
   return res;
}

string Device::valueNormalize(string value) {
  if(value=="1")return "ON";
  if(value=="0")return "OFF";
  return value;
} 

/*
bool Device::Describe() {
   stringstream desc;
   NameList input;
   input.init(control->get("event","trigger"));
   desc <<"{\"name\":\""<<name<<"\","<<endl;
   desc <<"  \"type\":\""<<get("type")<<"\","<<endl;
   desc <<"  \"active\":\""<<get("active")<<"\","<<endl;
   desc <<"  \"input\":[";
   for(auto i=0; i<input.size(); i++) {
	   auto pos = input[i].find("/")+1;
	   if(i>0) desc<<",";
	   desc<<"\""<<input[i].substr(pos)<<"\"";
   }
   desc <<"],"<<endl;
   desc <<"  \"output\":["<<endl<<"        ";
   for(auto i=0; i<component.size(); i++) {
	   if(i>0) desc<<","<<endl<<"        ";
	   desc<<component[i].Describe("json");
   }
   desc<<"]}";
   DynamicPage::Publish(name, desc.str(), "json");
   desc.str("");
   desc.clear();
   return true;
}
*/

bool Device::load(string& dir, string& nm) {
	NameList devlist;
	string config_dir = Conf::configdir+"/"+dir;
	FileManager::fileList(config_dir, devlist);
	for(size_t i=0; i<devlist.size(); i++) {
		string dname = FileManager::getStem(devlist[i]);
		if(dname=="control") {
			control = new Automaton(dir,dname);
		} else if(dname!=name && dname!="") {
			component.push_back(Component(dir,dname));
		}
	}
}

string Device::status() {
   string s="";
   for(size_t i=0; i<component.size(); i++) {
      if(s != "") s += "+";
      s += component[i].status();
   }
   return s;
}

string Device::status(string s) {
   NameList st;
   st.init(s);
   for(size_t i=0; i<st.size() && i<component.size(); i++) {
	component[i].add("status", "generic", st[i]);
	component[i].save();
   }
   return s;
}

string Device::statusAligne(string s, string devtype) {
  if(devtype == "onoff" && s.find("ON") !=string::npos && s.find("OFF") != string::npos) {
	  string res = "";
	  for(size_t i=0; i< component.size(); i++) {
		  if(i > 0) res += "+";
		  res+="OFF";
	  }
	  return res;
  }
  else return s;
}
  
bool Device::sync(string& topic, string& vv, string& tm) {
  string value = valueNormalize(vv);
  string cur_status = status();
  for(auto i=0; i<component.size(); i++) component[i].sync(topic, value, tm);
  if(cur_status != status()) {
     string tosend = "\""+status()+"\"";
     Logger::notify(get("event"),"\"event\"", tosend);
     Describe();
  }
  return true;
}

/*
string Device::aggregateValue(string value) {
   string res = "";
   for(size_t i=0; i<value.length(); i++)
     if(value[i]==' ') res+='+';
	else res+=value[i];
   return res;
}

string Device::valueNormalize(string value) {
  if(value=="1")return "ON";
  if(value=="0")return "OFF";
  return value;
} 
*/

bool Device::Describe() {
   stringstream desc;
   NameList input;
   input.init(control->get("event","trigger"));
   desc <<"{\"name\":\""<<name<<"\","<<endl;
   desc <<"  \"type\":\""<<get("type")<<"\","<<endl;
   desc <<"  \"active\":\""<<get("active")<<"\","<<endl;
   desc <<"  \"input\":[";
   for(auto i=0; i<input.size(); i++) {
	   auto pos = input[i].find("/")+1;
	   if(i>0) desc<<",";
	   desc<<"\""<<input[i].substr(pos)<<"\"";
   }
   desc <<"],"<<endl;
   desc <<"  \"output\":["<<endl<<"        ";
   for(auto i=0; i<component.size(); i++) {
	   if(i>0) desc<<","<<endl<<"        ";
	   desc<<component[i].Describe("json");
   }
   desc<<"]}";
   DynamicPage::Publish(name, desc.str(), "json");
   desc.str("");
   desc.clear();
   desc<<"<device name=\""<<name<<"\" active=\""<<get("active")<<"\">";
   desc<<"<input name=\""<<get("eventname")<<"\"/>";
   for(auto i=0; i<component.size(); i++) {
	   desc<<component[i].Describe("xml");
   }
   desc<<"</device>";
   DynamicPage::Publish(name, desc.str(), "xml");
   return true;
}

bool Device::sendStatus() {
   stringstream statusdesc;
   statusdesc << "[";
   for(auto i=0; i<component.size(); i++) {
	   if(i>0) statusdesc<<",";
	   statusdesc<<component[i].statusDescr();
   }
   statusdesc<<"]";
   Logger::notify(get("topic"), "\"status\"", statusdesc.str());
   return true;
}

string Device::setValue(string v) {
    string cmd;
    string res;
    if(get("active")=="OFF") return v;
    if((cmd=get(v,"cmd")) == "") {
	string info = "ERROR: "+get("name","generic")+" doesn't have command "+v;
	Logger::log("Component",info);
	add("active","OFF");
	return v;
    }
    Logger::notify(get("cmd"),"cmd",get(v,"cmd"));
    return v;
}

bool Device::getObservations(vector<string>& obs) {
    NameList cmds;
    NameList events;
    cmds.init(get("cmd", "trigger"), '+');
    events.init(get("event", "trigger"), '+');
    for(size_t i=0; i<events.size(); i++) obs.push_back(events[i]+"_event_execEvent");
    for(size_t i=0; i<cmds.size(); i++) obs.push_back(cmds[i]+"_cmd_execCmd");
    return true;
}

