#include "Logger.h"

using namespace std;

string Logger::log_theme = "*";
string Logger::status_theme = "*";
string Logger::notify_theme = "*";
string Logger::describe_theme = "*";
string Logger::log_msg = "*";
string Logger::status_msg = "*";
string Logger::notify_msg = "*";
string Logger::describe_msg = "*";

Logger::Logger(string file) {
   Params param(file);
   param.load();
   status_theme = param.get("status_theme");
   log_theme = param.get("log_theme");
   notify_theme = param.get("notify_theme");
   describe_theme = param.get("describe_theme");
   status_msg = param.get("status_msg");
   log_msg = param.get("log_msg");
   notify_msg = param.get("notify_msg");
   describe_msg = param.get("describe_msg");
   hsrv::strReplace(status_theme, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
   hsrv::strReplace(status_theme, "$MQTT_DOMAIN", Conf::mqtt_domain);
   if(Conf::mqtt_service!="" && Conf::mqtt_service != "*")
	   hsrv::strReplace(status_theme, "$MQTT_SERVICE", Conf::mqtt_service);

   hsrv::strReplace(log_theme, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
   hsrv::strReplace(log_theme, "$MQTT_DOMAIN", Conf::mqtt_domain);
   if(Conf::mqtt_service!="" && Conf::mqtt_service != "*")
	   hsrv::strReplace(log_theme, "$MQTT_SERVICE", Conf::mqtt_service);

   hsrv::strReplace(notify_theme, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
   hsrv::strReplace(notify_theme, "$MQTT_DOMAIN", Conf::mqtt_domain);
   if(Conf::mqtt_service!="" && Conf::mqtt_service != "*")
	   hsrv::strReplace(notify_theme, "$MQTT_SERVICE", Conf::mqtt_service);

   hsrv::strReplace(describe_theme, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
   hsrv::strReplace(describe_theme, "$MQTT_DOMAIN", Conf::mqtt_domain);
   if(Conf::mqtt_service!="" && Conf::mqtt_service != "*")
	   hsrv::strReplace(describe_theme, "$MQTT_SERVICE", Conf::mqtt_service);

}

bool Logger::log(string localname, string message) {
   string content = log_msg;
   hsrv::strReplace(content, "$LOCALNAME", localname); 
   hsrv::strReplace(content, "$MESSAGE", message); 
   Message msg(log_theme,content);
   Conf::mbx.Send("publisher", msg);
}

bool Logger::status(string localname, string message) {
   string theme = status_theme;
   string content = status_msg;
   hsrv::strReplace(theme, "$LOCALNAME", localname); 
   hsrv::strReplace(content, "$MESSAGE", message); 
   Message msg(theme,content);
   Conf::mbx.Send("publisher", msg);
}

bool Logger::notify(string topic, string infotype, string message) {
   string content = notify_msg;
   string theme = topic;
   hsrv::strReplace(content, "$INFOTYPE", infotype); 
   hsrv::strReplace(content, "$MESSAGE", message); 
   Message msg(theme,content);
   Conf::mbx.Send("publisher", msg);
}

bool Logger::describe(string topic, string message) {
   string content = describe_msg;
   string theme = describe_theme;
   hsrv::strReplace(theme, "$TOPIC", topic); 
   hsrv::strReplace(content, "$MESSAGE", message); 
   Message msg(theme,content);
   Conf::mbx.Send("publisher", msg);
}
