#include "Rule.h"
#define THR 0.5

using namespace std;

//___________________ Class Input _____________

Input::Input(string tp, string df) {
   topic = tp;
   updtime = deadline = 0.0;
   defvalue = hsrv::a2double(df);
   value = defvalue;
   status = "ON";
}

bool Input::match(double v) {
   double x, y;
   if(status == "OFF") return false;
   x = v<0?-v:v;
   y = value<0? -value:value;
   return x-y<THR;
}

double Input::reset() {
   value = defvalue;
   return value;
}

bool Input::getTopic(vector<string>& obs) {
   obs.push_back(topic);
   return true;
}

string Input::description(string lang) {
   string desc;
   return desc;
}

bool Input::connect(map<string,vector<Input*>>*) {
	return false;
}

bool Input::update(string& val) {
	return true;
}

bool Input::checkDeadline() {
	return true;
}

bool Input::isActive() {
	return false;
}

bool Input::print() {
   cout<<"Input: "<<name<<", Topic: "<<topic<<", value: "<<value;
   cout<<", default: "<<defvalue<<", status: "<<status<<endl;
   return true;
}

string Input::Describe(string language) {
   string descr;
   if(language=="json") {
      descr = "{\"name\":\""+name+"\", \"alias\":\""+alias+"\"}";
   }
   else {
      descr = "      <item name=\""+name+"\" alias=\""+alias+"\"/>\n";
   }
   return descr;
}


//________________________ Class Condition______________________


Condition::Condition(Rule* r, string tp, string df): Input::Input(tp, df) {
   rule = r;
   //name = r->name+FileManager::getTail(tp);
   auto pos=tp.find("from/");;
   if(pos!=string::npos) name = tp.substr(pos+5);
   else name = tp;
   duration = -1;
}

bool Condition::connect(map<string,vector<Input*>>* ctab) {
   if(ctab->count(topic) == 0) {
	   vector<Input*> tmp;
	   tmp.push_back(this);
	   (*ctab)[topic]=tmp;
	}
   	else (*ctab)[topic].push_back(this);
   return true;
}

bool Condition::update(string& val) {
   value = hsrv::a2double(val);
   updtime = hsrv::gettime();
   rule->update(name);
   return true;
}

bool Condition::isActive() {
	return value != defvalue;
}

//_________________________ Class Event __________________________
//


Event::Event(Rule* r, string tp, string dv, string dur): Input::Input(tp, dv) {
   rule = r;
   //name = r->name+FileManager::getTail(tp);
   auto pos=tp.find("from/");;
   if(pos!=string::npos) name = tp.substr(pos+5);
   else name = tp;
   duration = hsrv::a2double(dur);
}

bool Event::connect(map<string,vector<Input*>>* ctab) {
   if(ctab->count(topic) == 0) {
	   vector<Input*> tmp;
	   tmp.push_back(this);
	   (*ctab)[topic]=tmp;
	}
   	else (*ctab)[topic].push_back(this);
   return true;
}

bool Event::update(string& val) {
   double tvalue = hsrv::a2double(val);
   if(tvalue != defvalue) {
	   value = tvalue;
           updtime = hsrv::gettime();
           deadline = updtime+duration;
           rule->update(name);
   }
   return true;
}

bool Event::checkDeadline() {
   double ctime = hsrv::gettime();
   if(ctime > deadline) {
       updtime = ctime;
       value = defvalue;
       return true;
   }
   return false;
}

bool Event::isActive() {
   checkDeadline();
   return value != defvalue;
}
