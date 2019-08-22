/*
 * Component.cpp
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */
#include "Component.h"

using namespace std;

Component::Component(string dir, string nm): Params::Params(dir, nm) {
	load();
	last_update = 0;
	check();
}

string Component::name() {
	return get("name","generic");
}

string Component::status() {
	return get("status","generic");
}

string Component::active() {
	return lmode;
}

string Component::setValue(string v) {
	string cmd;
	string res;
	if(get("active")=="OFF") return v;
	if((cmd=get(v,"cmd")) == "") {
		string info = "ERROR: "+get("name","generic")+" doesn't have command "+v;
		Logger::log("Component",info);
		add("active","OFF");
		return v;
	}
	if(get("status") == v) return v;
	else Logger::notify(get("cmd"),"cmd",get(v,"cmd"));
	return v;
}

bool Component::getSubjects(Params& subj) {
	subj.add(name(),"subject", "cmd");
}

bool Component::getObservations(vector<string>& obs) {
	obs.push_back(get("event","trigger")+"_event_sync");
	obs.push_back(get("event","trigger")+"_status_sync");
}

bool Component::check() {
  return true;
}

bool Component::sync(string& topic, string& value, string& tm) {
  if(get("event","trigger") == topic) {
    double tt = hsrv::a2double(tm);
    if(tt < last_update) return false;
    last_update = tt;
    add("status","generic", value);
    return true;
  }
  else return false;
}

string Component::getValue(string& body, string word) {
   string tmp = "";
   size_t pos=body.find(word, 0);
   if(pos == string::npos) return "";
   pos+= word.size();
   if(pos >= body.size()) return "";
   while(pos<body.size() && body[pos] != '"') pos++;
   pos++;
   while(pos<body.size() && body[pos] != '"') tmp += body[pos++];
   return tmp;
}

string Component::Describe(string language) {
   string descr;
   string gpio = get("cmd");
   auto pos = gpio.find("/")+1;
   gpio = get("cmd").substr(pos);
   if(language == "json") {
      descr = "{\"name\":\""+gpio+"\",\"alias\":\""+name()+"\",\"status\":\""+get("status")+"\"}";
   }
   else {
      descr = "<comp name=\""+name()+"\" status="+"\""+get("status")+"\"/>";
   }
   return descr;
}

string Component::statusDescr() {
   string descr = "{\""+name()+"\":\""+get("status")+"\"}";
   return descr;
}
