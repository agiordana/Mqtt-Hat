#include "Gpio.h"

using namespace std;

NameList Gpio::initDone;

Gpio::Gpio(string dir, string file): Params::Params(dir, file) {
    load();
    string ev = get("event");
    // replace the real domain and subdomain in the event theme
    hsrv::strReplace(ev, "$MQTT_DOMAIN", Conf::mqtt_domain);
    hsrv::strReplace(ev, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
    add("event", ev);
    name = get("capename");
    gpioname = get("gpioname");
    gpiotype = get("type");
    gpiopath = get("gpiopath");
    gpiovalue = gpiopath+"/value";
    pathname = "";
    wait_until = 0;
    string waitfor = get("waitfor");
    if(waitfor == "") w = 1;
    else w = hsrv::a2double(waitfor);
    currentValue=get("currentValue");
    if(currentValue=="") currentValue = get("default_value");
    if(currentValue=="") currentValue = "0";
}


bool Gpio::initialise() {
    setof<Attribute> init;
    string gpiopath = get("gpiopath");
    string cmd;
    string res;
    setof<Attribute>::iterator it;
    init = extractCategory("init");
    if(!FileManager::isDir(gpiopath) && init.size()>0) {
       for(it=init.begin(); it!=init.end(); it++) {
          cmd = it->value;
          if(!initDone.member(cmd)) {
             res = hsrv::cmdExec(cmd);
             initDone.add(cmd);
             sleep(1);
          }
       }
    }
    return true;
}

bool Gpio::program() {
    setof<Attribute> prg;
    string cmd;
    string res;
    setof<Attribute>::iterator it;
    prg = extractCategory("script");
    for(it=prg.begin(); it!=prg.end(); it++) {
        cmd = it->value;
        res = hsrv::cmdExec(cmd);
    }
    return true;
}


string Gpio::getpath(string value) {
  if(gpiotype=="booleanout") return gpiovalue;
}

string Gpio::get_and_set_value(string value) {
  if(value=="") return currentValue;
  if(value == "OFF" || value == "off") {
      set("mode", "OFF");
      save();
   }
  if(value == "ON" || value == "on") currentValue = "1";
  else if(value == "OFF" || value == "off") currentValue = "0";
  else currentValue = value;
  add("currentValue", currentValue);
  save();
  return currentValue;
}

bool Gpio::setInitialValue() {
  setCurrentValue("");
  if(get("mode") == "OFF") setCurrentValue("OFF");
  else setCurrentValue("0");
  return true;
}

bool Gpio::setCurrentValue(std::string value) {
   string v = get_and_set_value(value);
   string out = getpath(value);
   FILE *chout;
   chout = fopen(out.c_str(), "w");
   if(chout == NULL) return false;
   fprintf(chout, "%s", v.c_str());
   fclose(chout);
   //hsrv::publish(name, currentValue);
   return true;
}

string Gpio::jsonDescribe() {
   stringstream descr;
   descr<<"{\"name\":\""<<name<<"\", \"gpio\":\""<<gpioname<<"\",\"type\":\""<<gpiotype<<"\", \"value\":"<<currentValue<<"}";
   return descr.str();
}

string Gpio::xmlDescribe() {
   stringstream descr;
   descr<<"<item name=\""<<name<<"\"  gpio=\""<<gpioname<<"\" type=\""<<gpiotype<<"\" value=\""<<currentValue<<"\"/>";
   return descr.str();
}
