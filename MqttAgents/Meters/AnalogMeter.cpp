#include "AnalogMeter.h"
#define INITIALSILENCE 5

using namespace std;

AnalogMeter::AnalogMeter(string dir, string n): Params(dir, n) {
    load();
    name = n;
    in = get("input");
    source = "from/"+in;
    topic = get("event");
    type = get("type");
    probe = get("probe");
    currentTime = hsrv::gettime();
    if(get("status")!="") status = get("status");
    else status = "OFF";
    string sf = get("scale_factor");
    scale_factor = hsrv::a2double(sf);
    unit = get("unit");
    currentValue = "0";
}

bool AnalogMeter::execCmd(string& topic, string& vv, string& tm) {
	    return true;
}


bool AnalogMeter::execEvent(string& tp, string& value, string& tm) {
    double delta = 0;
    double tt = hsrv::gettime();
    if(tp == source) {
        string tvalue = remap(value);
        currentValue = tvalue;
        currentTime = tt;
        Describe();
	sendStatus();
	return true;
    }
    return false;
}


string AnalogMeter::remap(string& value) {
    double rvalue = hsrv::a2double(value);
    if(probe==""||probe == "default") rvalue *= scale_factor;
    else if(probe=="Tprobe_v12") rvalue = tprobe_v12(rvalue, scale_factor);
    string tvalue = hsrv::double2a(rvalue);
    if(tvalue == "") return value;
    else return tvalue;
}

double AnalogMeter::tprobe_v12(double& a, double& bias) {
   int B=3450+bias;
   double R = 1023.0/((float)a) -1.0;
   double temperature=1.0/(log(R)/B+1/298.15)-273.15;
   int itemp = (int)(temperature*10);
   return (double)itemp/10;
}

bool AnalogMeter::Describe() {
    stringstream desc;
    // publishing JSON description
    desc <<"{\"name\":\""<<name<<"\","<<endl;
    desc <<"  \"type\":\""<<type<<"\","<<endl;
    desc <<"  \"status\":\""<<status<<"\","<<endl;
    desc <<"  \"input\":\""<<in<<"\","<<endl;
    desc <<"  \"value\":"<<currentValue<<","<<endl;
    desc <<"  \"unit\":\""<<unit<<"\","<<endl;
    desc <<"  \"time\":\""<<hsrv::double2a(currentTime)<<"\""<<endl;
    desc <<"}";
    DynamicPage::Publish(name, desc.str(), "json");
    // publishing XML description
    desc.str("");
    desc.clear();
    desc <<"<device name=\""<<name<<"\" "<<"type=\""<<type<<"\" "<<"status:\""<<status<<"\" >"<<endl;
    desc <<"<input name=\""<<in<<"\"/>"<<endl;
    desc <<"<value value=\""<<currentValue<<"\" unit=\""<<unit<<"\"/>";
    desc <<"<time value=\""<<hsrv::double2a(currentTime)<<"\"/>";
    desc <<"</device>";
    DynamicPage::Publish(name, desc.str(), "xml");
    return true;
}

bool AnalogMeter::sendStatus() {
    stringstream desc;
    desc <<"{\"name\":\""<<name<<"\",";
    desc <<"  \"type\":\""<<type<<"\",";
    desc <<"  \"status\":\""<<status<<"\",";
    desc <<"  \"input\":\""<<in<<"\",";
    desc <<"  \"value\":"<<currentValue<<",";
    desc <<"  \"unit\":\""<<unit<<"\",";
    desc <<"  \"time\":\""<<hsrv::double2a(currentTime)<<"\"";
    desc <<"}";
    Logger::notify(topic, "\"status\"", desc.str());
    return true;
}

bool AnalogMeter::reset() {
    currentValue = "0";
    currentTime = hsrv::gettime();
    Describe();
    sendStatus();
    return true;
}

bool AnalogMeter::sync() {
    Describe();
    sendStatus();
    return true;
}
