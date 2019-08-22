#include "ElectricMeter.h"
#define INITIALSILENCE 5

using namespace std;

ElectricMeter::ElectricMeter(string dir, string n): Params(dir, n) {
    load();
    name = n;
    in = get("input");
    source = "from/"+in;
    topic = get("event");
    type = get("type");
    string tmp = get("alpha");
    if(tmp=="") alpha = 0;
    else alpha = hsrv::a2double(tmp);
    tmp = get("quantum");
    if(tmp=="") quantum = 1;
    else quantum = hsrv::a2double(tmp);
    tmp = get("unit");
    if(tmp=="") unit = "Wh";
    else unit = tmp;
    energy = Ipower = Apower = Wpower = 0;
    if((tmp = get("energy"))!="") energy = hsrv::a2double(tmp);
    if((tmp = get("deltaEnergy"))!="") deltaEnergy = hsrv::a2double(tmp);
    currentTime = origin = hsrv::gettime();
    if(get("status")!="") status = get("status");
	else status = "ON";
    deltaEnergy = 0;
}

bool ElectricMeter::execCmd(string& topic, string& vv, string& tm) {
    return true;
}

bool ElectricMeter::execEvent(string& tp, string& value, string& tm) {
    double delta = 0;
    double tt = hsrv::gettime();
    if(tp == source && value == "1") {
	double n = hsrv::a2double(value);
	delta = quantum * n;
	if(tt<origin+INITIALSILENCE) return false;
        energy += delta;
	deltaEnergy += delta;
	return true;
    }
    return false;
}

bool ElectricMeter::sync() {
    double tt = hsrv::gettime();
    Apower = (energy/(tt-origin))*3600;
    Ipower = Ipower * (1 - alpha) + alpha * (deltaEnergy/(tt - currentTime)) * 3600;
    currentTime = tt;
    deltaEnergy = 0;
    Describe();
    sendStatus();
    return true;
}

bool ElectricMeter::reset() {
    energy = 0;
    origin = hsrv::gettime()-1;
    currentTime = hsrv::gettime();
    deltaEnergy = 0;
    Ipower = Apower = 0;
    Describe();
    sendStatus();
    return true;
}

bool ElectricMeter::sendStatus() {
   stringstream desc;
   // publishing JSON description
   desc <<"{\"name\":\""<<name<<"\",";
   desc <<"  \"type\":\""<<type<<"\",";
   desc <<"  \"status\":\""<<status<<"\",";
   desc <<"  \"input\":\""<<in<<"\",";
   desc <<"  \"energy\":"<<hsrv::double2a(energy)<<",";
   desc <<"  \"Ipower\":"<<hsrv::double2a(Ipower)<<",";
   desc <<"  \"Apower\":"<<hsrv::double2a(Apower)<<",";
   desc <<"  \"unit\":\""<<unit<<"\",";
   desc <<"  \"value\":\""<<hsrv::double2a(Ipower)<<"\",";
   desc <<"  \"time\":\""<<hsrv::double2a(currentTime)<<"\"";
   desc <<"}";
   Logger::notify(topic, "\"status\"", desc.str());
   return true;
}

bool ElectricMeter::Describe() {
   stringstream desc;
   // publishing JSON description
   desc <<"{\"name\":\""<<name<<"\","<<endl;
   desc <<"  \"type\":\""<<type<<"\","<<endl;
   desc <<"  \"status\":\""<<status<<"\","<<endl;
   desc <<"  \"input\":\""<<in<<"\","<<endl;
   desc <<"  \"energy\":"<<hsrv::double2a(energy)<<","<<endl;
   desc <<"  \"Ipower\":"<<hsrv::double2a(Ipower)<<","<<endl;
   desc <<"  \"Apower\":"<<hsrv::double2a(Apower)<<","<<endl;
   desc <<"  \"unit\":\""<<unit<<"\","<<endl;
   desc <<"  \"time\":\""<<hsrv::double2a(currentTime)<<"\""<<endl;
   desc <<"}";
   DynamicPage::Publish(name, desc.str(), "json");
   // publishing XML description
   desc.str("");
   desc.clear();
   desc <<"<device name=\""<<name<<"\" "<<"type=\""<<type<<"\" "<<"status=\""<<status<<"\" >"<<endl;
   desc <<"<input name=\""<<in<<"\">"<<endl;
   desc <<"<energy value=\""<<hsrv::double2a(energy)<<"\">";
   desc <<"<Ipower value=\""<<hsrv::double2a(Ipower)<<"\">";
   desc <<"<Apower value=\""<<hsrv::double2a(Apower)<<"\">";
   desc <<"<unit value=\""<<unit<<"\">";
   desc <<"<time value=\""<<hsrv::double2a(currentTime)<<"\">";
   desc <<"</device>";
   DynamicPage::Publish(name, desc.str(), "xml");

   return true;
}

