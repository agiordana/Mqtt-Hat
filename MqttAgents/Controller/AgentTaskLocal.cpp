#include "AgentTaskLocal.h"


using namespace std;

RuleHub AgentTaskLocal::rule;

void AgentTaskLocal::EventProcessor(std::string) {
  MsgQueue<Message>* eventsource = Conf::mbx.Attach("evprocessor");
  while(true) {
	  Message m = eventsource->receive(); 
	  Logger::log("EventExecutor", "Event Arrived");
  }
}

void AgentTaskLocal::CmdExecutor(string id) {
  Logger::log("Controller", "Started");
  rule.Load("rules");
  //rule.connect(condition, event);
  MsgQueue<Message>* cmdsource = Conf::mbx.Attach("cmdexecutor");
  while(true) {
       Message msg = cmdsource->receive();
       string name;
       string value;
       string todo;
       string key = msg.theme;
       string lastfield = FileManager::getTail(msg.theme);
       if(lastfield=="newrule") {
	       Configure(msg.content);
	       rule.Reload("rules");
       }
       else if(lastfield=="ruledelete") {
	       Delete(msg.content);
	       rule.Reload("rules");
       }
       else if(lastfield!="description"&&lastfield!="logger"){
         decode(msg, todo, name, value);
         if(todo == "request") {
	       if(value=="description"||value=="description.json") sendDescription("json");
	       else if(value=="description.xml") sendDescription("xml");
	       else if(key=="local/trigger" && value=="upd") rule.timeUpdate();
         }
         else if(todo=="event" && rule.count(key) >0) {
	    for(auto i=0; i<rule[key].size(); i++) rule[key][i]->update(value);
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
   stringstream aliases;
   ss<<message;
   boost::property_tree::ptree pt, truecond, falsecond, clause, target;
   read_json(ss, pt);
   
   string rname = pt.get<string>("name");
   string rtype = pt.get<string>("type");
   string rfrom = pt.get<string>("from");
   string rto = pt.get<string>("to");
   truecond = pt.get_child("enabling_true_conditions");
   falsecond = pt.get_child("enabling_false_conditions");
   string rrepeat="";
   string ractionactive="";
   string rminev="";
   string rduration="";
   string reventduration="";
   if(rtype == "MONOSTABLE") {
	   clause = pt.get_child("clause");
	   rminev = pt.get<string>("min_conditions");
	   rduration = pt.get<string>("duration");
	   reventduration = pt.get<string>("event_duration");
   }
   else {
	   rrepeat = pt.get<string>("repeat_every");
   }
   target = pt.get_child("action_target");
   string ractionon = pt.get<string>("action_on");
   if(rtype == "BISTABLE") {
	   ractionactive = pt.get<string>("action_active");
   }
   string ractionoff = pt.get<string>("action_off");
   mkManifest(rname, message);
   ifstream ruletemplate;
   ofstream rule;
   string rulefile;
   string description;
   char ch;
   string templatepath = Conf::configdir+"/templates/";
   if(rtype == "MONOSTABLE") templatepath += "monostable.xml";
   else if(rtype == "BISTABLE") templatepath += "bistable.xml";
   else return false;
   rulefile = Conf::configdir+"/rules/"+rname+".xml";
   // loads the devrce template
   ruletemplate.open(templatepath.c_str());
   if(!ruletemplate.is_open()) return false;
   while((ch=ruletemplate.get())&&!ruletemplate.eof()) description+=ch;
   ruletemplate.close();
   
   // create the device main description;
   rule.open(rulefile.c_str());
   if(!rule.is_open()) return false;
   while(hsrv::strReplace(description, "$NAME", rname));
   while(hsrv::strReplace(description, "$TYPE", rtype));
   while(hsrv::strReplace(description, "$MQTT_DOMAIN", Conf::mqtt_domain));
   while(hsrv::strReplace(description, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain));
   while(hsrv::strReplace(description, "$MQTT_SERVICE", Conf::mqtt_service));
   while(hsrv::strReplace(description, "$FROM", rfrom));
   while(hsrv::strReplace(description, "$TO", rto));
   while(hsrv::strReplace(description, "$ACTIONON", ractionon));
   while(hsrv::strReplace(description, "$ACTIONOFF", ractionoff));
   if(rtype == "BISTABLE"){
	   while(hsrv::strReplace(description, "$ACTIONACTIVE", ractionactive));
	   while(hsrv::strReplace(description, "$REPEATEVERY", rrepeat));
   }
   if(rtype == "MONOSTABLE") {
	   while(hsrv::strReplace(description, "$MINEV", rminev));
	   while(hsrv::strReplace(description, "$DURATION", rduration));
	   while(hsrv::strReplace(description, "$EVENTDURATION", reventduration));
   }
   string rtruecond = "";
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, truecond) {
	   if(rtruecond != "") rtruecond += "+";
           rtruecond += "from/";
           string truename = v.second.get<string>("name");
           rtruecond += truename;
           string aliasname = v.second.get<string>("alias");
	   aliases<<"<attribute name=\""<<truename<<"\" category=\"alias\" value=\""<<aliasname<<"\"/>"<<endl;

   }
   while(hsrv::strReplace(description, "$TRUECONDITIONS", rtruecond));
   string rfalsecond = "";
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, falsecond) {
	   if(rfalsecond != "") rfalsecond += "+";
           rfalsecond += "from/";
           string truename = v.second.get<string>("name");
           rfalsecond += truename;
           string aliasname = v.second.get<string>("alias");
	   aliases<<"<attribute name=\""<<truename<<"\" category=\"alias\" value=\""<<aliasname<<"\"/>"<<endl;
   }
   while(hsrv::strReplace(description, "$FALSECONDITIONS", rfalsecond));
   if(rtype == "MONOSTABLE") {
      string rclause = "";
      BOOST_FOREACH(boost::property_tree::ptree::value_type &v, clause) {
	   if(rclause != "") rclause += "+";
           rclause += "from/";
           string truename = v.second.get<string>("name");
           rclause += truename;
           string aliasname = v.second.get<string>("alias");
	   aliases<<"<attribute name=\""<<truename<<"\" category=\"alias\" value=\""<<aliasname<<"\"/>"<<endl;
      }
      while(hsrv::strReplace(description, "$EVENTS", rclause));
   }

   string rtarget = "";
   BOOST_FOREACH(boost::property_tree::ptree::value_type &v, target) {
	   if(rtarget != "") rtarget += "+";
           rtarget += "to/";
           string truename = v.second.get<string>("name");
           rtarget += truename;
           string aliasname = v.second.get<string>("alias");
	   aliases<<"<attribute name=\""<<truename<<"\" category=\"alias\" value=\""<<aliasname<<"\"/>"<<endl;
   }
   while(hsrv::strReplace(description, "$TARGET", rtarget));
   hsrv::strReplace(description, "$ALIAS", aliases.str());

   for(auto i=0; i<description.size(); i++) rule.put(description[i]);
   rule.close();

   return true;
}

bool AgentTaskLocal::Delete(string& message) {
   stringstream ss;
   ss<<message;
   boost::property_tree::ptree pt;
   read_json(ss, pt);
   string rname = pt.get<string>("name");
   string rfname = rname +".json";
   string xrfname = rname+".xml";
   string manifestpath = Conf::configdir+"/rulemanifest";
   string rulepath = Conf::configdir+"/rules/"+rname+".xml";
   string ruledir = Conf::configdir+"/rules";
   if(!FileManager::isFile(rulepath)) return false;
   FileManager::deleteFile(manifestpath, rfname);
   FileManager::deleteFile(ruledir, xrfname);
   return true;
}

bool AgentTaskLocal::mkManifest(string& rname, string& message) {
   ofstream manifest;
   string manifestpath = Conf::configdir+"/rulemanifest/";
   if(!FileManager::isDir(manifestpath)) FileManager::makeDir(manifestpath, true);
   manifestpath += rname;
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
