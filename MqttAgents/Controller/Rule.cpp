#include "Rule.h"

using namespace std;

Rule::Rule(RuleHub* hub, string dir, string nm): Params::Params(dir, nm) {
   load();
   ctime = hsrv::gettime();
   name = nm;
   ty = get("rule_type");
   NameList tmp;
   tmp.init(get("enabling_true_condition"), '+');
   for(auto i=0; i<tmp.size(); i++) {
	   tcond.push_back(new Condition(this, tmp[i], "0"));
	   hub->condition.push_back(tcond.back());
	   (tcond.back())->alias = get(tcond.back()->name,"alias");
   }
   tmp.clear();
   tmp.init(get("enabling_false_condition"), '+');
   for(auto i=0; i<tmp.size(); i++) {
	   fcond.push_back(new Condition(this, tmp[i], "1"));
	   hub->condition.push_back(fcond.back());
	   (fcond.back())->alias = get(fcond.back()->name,"alias");
   }
   tmp.clear();
   tmp.init(get("clause"), '+');
   for(auto i=0; i<tmp.size(); i++) {
	   ev.push_back(new Event(this, tmp[i], "0", get("event_duration")));
	   hub->event.push_back(ev.back());
	   (ev.back())->alias = get(ev.back()->name,"alias");
   }
   if(ty == "MONOSTABLE") behavior = new MonostableBehavior(this);
   else if(ty == "BISTABLE") behavior = new BistableBehavior(this);
   else behavior = NULL;
   if(behavior != NULL) status = get("status");
   else status = "OFF";
}

bool Rule::update(string v) {
	if(status == "ON" ) return behavior->update(v);
	return true;
}

bool Rule::update() {
   if(status == "ON" && behavior->active) return behavior->update();
   return true;
}

bool Rule::print() {
   cout<<"Rule: "<<name<<", desc: "<<xmlEncode(1)<<endl;
   return true;
}

bool Rule::Describe() {
   stringstream desc;
   NameList tg;
   tg.init(get("action_target"), '+');
   desc <<"{\"name\":\""<<name<<"\","<<endl;
   desc <<"  \"type\":\""<<get("rule_type")<<"\","<<endl;
   desc <<"  \"from\":\""<<get("from")<<"\","<<endl;
   desc <<"  \"to\":\""<<get("to")<<"\","<<endl;
   desc <<"  \"enabling_true_condition\":[";
   for(auto i=0; i<tcond.size(); i++) {
   	   if(i>0) desc<<",";
   	   desc<<tcond[i]->Describe("json");
   }
   desc <<"],"<<endl;
   desc <<"  \"enabling_false_condition\":[";
   for(auto i=0; i<fcond.size(); i++) {
   	   if(i>0) desc<<",";
   	   desc<<fcond[i]->Describe("json");
   }
   desc <<"],"<<endl;
   if(get("rule_type") == "MONOSTABLE") {
     desc <<"  \"clause\":[";
     for(auto i=0; i<ev.size(); i++) {
   	   if(i>0) desc<<",";
   	   desc<<ev[i]->Describe("json");
     }
     desc <<"],"<<endl;
     desc <<"  \"duration\":\""<<get("duration")<<"\","<<endl;
     desc <<"  \"event_duration\":\""<<get("event_duration")<<"\","<<endl;
     desc <<"  \"min_conditions\":\""<<get("min_conditions")<<"\","<<endl;
     
   }
   else {
      desc <<"  \"repeat_every\":\""<<get("repeat_every")<<"\","<<endl;
   }
   desc<<behavior->Describe("json");
   if(get("rule_type") == "BISTABLE") {
      desc <<"  \"action_active\":\""<<get("action_active")<<"\","<<endl;
   }
   desc <<"  \"action_on\":\""<<get("action_on")<<"\","<<endl;
   desc <<"  \"action_off\":\""<<get("action_off")<<"\","<<endl;
   desc <<"  \"status\":\""<<status<<"\""<<endl;
   desc<<"}";
   DynamicPage::Publish(name, desc.str(), "json");
   desc.str("");
   desc.clear();
   desc<<"<rule name=\""<<name<<"\" type=\""<<get("rule_type")<<"\" status=\""<<status<<"\" >"<<endl;
   desc<<"    <active from=\""<<get("from")<<"\" to=\""<<get("to")<<"\" />"<<endl;
   desc<<"    <enabling_true_condition>"<<endl;
   for(auto i=0; i<tcond.size(); i++) desc<<tcond[i]->Describe("xml");
   desc<<"    </enabling_true_condition>"<<endl;
   desc<<"    <enabling_false_condition>"<<endl;
   for(auto i=0; i<fcond.size(); i++) desc<<fcond[i]->Describe("xml");
   desc<<"    </enabling_false_condition>"<<endl;
   if(get("rule_type")=="MONOSTABLE") {
        desc<<"    <clause>"<<endl;
   	for(auto i=0; i<ev.size(); i++) desc<<ev[i]->Describe("xml");
        desc<<"    </clause>"<<endl;
	desc<<"    <duration value=\""<<get("duration")<<"\"/>"<<endl;
	desc<<"    <event_duration value=\""<<get("event_duration")<<"\"/>"<<endl;
	desc<<"    <min_conditions value=\""<<get("min_conditions")<<"\"/>"<<endl;
   }
   else {
	desc<<"    <repeat_every value=\""<<get("repeat_every")<<"\"/>"<<endl;
   }
   desc<<behavior->Describe("xml");
   if(get("rule_type") == "BISTABLE") {
        desc <<"    <action_active value=\""<<get("action_active")<<"\">"<<endl;
   }
   desc <<"    <action_on value=\""<<get("action_on")<<"\">"<<endl;
   desc <<"    <action_off value=\""<<get("action_off")<<"\">"<<endl;
   desc <<"</rule>";
   DynamicPage::Publish(name, desc.str(), "xml");
   return true;
}

//_____________________ Class Behavior ______________________________

bool Behavior::isTimeEnabled() {
   string daytime = hsrv::getasciitimeday();
   double secdaytime = hsrv::string2second(daytime);
   if(from<0 || to <0) return true;
   if(from < to && from <= secdaytime && secdaytime <= to) return true;
   if(from > to && (from <= secdaytime || secdaytime <= to)) return true;
   return false;
}

string Behavior::Describe(string language) {
   stringstream descr;
   string name;
   if(language == "json") {
      descr << "  \"action_target\": [";
      for(auto i=0; i<target.size(); i++) {
	 auto pos=target[i].find("to/");
	 if(pos != string::npos) name = target[i].substr(pos+3);
	 else name = target[i];
	 if(i>0) descr<<",";
	 descr << "{\"name\":\""<<name<<"\",\"alias\":\""<<r->get(name,"alias")<<"\"}";
      }
      descr<<"],"<<endl;
   }
   else if(language=="xml") {
      descr<<"    <action_target>"<<endl;
      for(auto i=0; i<target.size(); i++) {
	  auto pos=target[i].find("to/");
	  if(pos != string::npos) name = target[i].substr(pos+3);
	  else name = target[i]; 
	  descr<<"      <item name=\""<<name<<"\" alias=\""<<r->get(name,"alias")<<"\"/>"<<endl;
      }
      descr<<"    </action_target>"<<endl;
   }
   return descr.str();
}


bool Behavior::isConditionActive() {
   bool cond_active = true;
   bool time_active = isTimeEnabled();
   for(auto i=0; i<r->tcond.size()&&cond_active; i++) if(!r->tcond[i]->isActive()) cond_active = false;
   for(auto i=0; i<r->fcond.size()&&cond_active; i++) if(!r->fcond[i]->isActive()) cond_active = false;
   return cond_active;
}

bool Behavior::update(string v) {
	cout<<"Virtual upd"<<endl;
	return true;
}


bool Behavior::update() {
	cout<<"Virtual upd1"<<endl;
	return true;
}

bool Behavior::sendAction(string value) {
   for(auto i=0; i<target.size(); i++) {
      Logger::notify(target[i],"cmd",value);
   }
   return true;
}

//_____________________ Class MonostableBehavior ______________________________

MonostableBehavior::MonostableBehavior(Rule* par) {
   string tt = par->get("from");
   if(tt!="") from = hsrv::string2second(tt);
       else from = -1;
   tt = par->get("to");
   if(tt!="") to = hsrv::string2second(tt);
       else to = -1;
   ctime = hsrv::gettime();
   tt = par->get("min_conditions");
   if(tt == "") minev = (int) par->ev.size();
   else minev = hsrv::a2int(tt);
   tt = par->get("duration");
   if(tt == "") duration = 0;
   else duration = hsrv::a2double(tt);
   waituntil = 0;
   target.init(par->get("action_target"), '+');
   action_on = par->get("action_on");
   action_off = par->get("action_off");
   active = false;
   r = par;
}


bool MonostableBehavior::update(string in) {
   bool cond_active = true;
   if(!isTimeEnabled()) {
	   if(active) {
		active = false;
	   	sendAction(action_off);
	   }
	   return true;
   };
   for(auto i=0; i<r->tcond.size()&&cond_active; i++) {
	   if(!r->tcond[i]->isActive()) cond_active = false;
   }
   for(auto i=0; i<r->fcond.size()&&cond_active; i++) {
	   if(!r->fcond[i]->isActive()) cond_active = false;
   }
   if(!cond_active) {
	   if(active) {
		 active = false;
		 sendAction(action_off);
	   }
	   return true;
   }
   size_t n=0;
   for(auto i=0; i<r->ev.size(); i++) if(r->ev[i]->isActive()) n++;
   if(n>=minev) {
	   waituntil = ctime + duration;
	   if(!active) {
		  active = true;
		  sendAction(action_on);
	   } 
   }
   return true;
}

bool MonostableBehavior::update() {
   ctime = hsrv::gettime();
   if(active && ctime > waituntil) {
	   sendAction(action_off);
	   active = false;
   }
   return true;
}


//_____________________ Class BistableBehavior ______________________________

BistableBehavior::BistableBehavior(Rule* par) {
   string tt = par->get("from");
   if(tt!="") from = hsrv::string2second(tt);
       else from = -1;
   tt = par->get("to");
   if(tt!="") to = hsrv::string2second(tt);
       else to = -1;
   ctime = hsrv::gettime();
   tt = par->get("repeat_every");
   if(tt == "") repeat_every = 0;
   else repeat_every = hsrv::a2double(tt);
   target.init(par->get("action_target"), '+');
   action_on = par->get("action_on");
   action_active = par->get("action_active");
   action_off = par->get("action_off");
   active = false;
   r = par;
}


bool BistableBehavior::update(string in) {
   bool cond_active = true;
   ctime = hsrv::gettime();
   bool time_active = isTimeEnabled();
   for(auto i=0; i<r->tcond.size()&&cond_active; i++) if(!r->tcond[i]->isActive()) cond_active = false;
   for(auto i=0; i<r->fcond.size()&&cond_active; i++) if(!r->fcond[i]->isActive()) cond_active = false;
   if(time_active && cond_active && !active) {
   	   active = true;
	   sendAction(action_on);
   }
   else if(active && (!time_active || !cond_active)) {
	   active = false;
	   sendAction(action_off);
   }
   if(active && repeat_every != 0) waituntil = ctime + repeat_every;
   return true;
}

bool BistableBehavior::update() {
   if(active && repeat_every > 0) {
	   ctime = hsrv::gettime();
	   if(ctime > waituntil) {
		   sendAction(action_active);
		   waituntil = ctime+repeat_every;
	   }
   }
   return true;
}

