#include "DynamicPage.h"

using namespace std;

mutex DynamicPage::DPmutex;

map<string,string> DynamicPage::page;
map<string,string> DynamicPage::xmlpage;

string DynamicPage::Index(string key, string language) {
	if(language == "json") return jsonIndex(key);
	else if(language== "xml") return xmlIndex(key);
	else return "Not Found";
}

bool DynamicPage::Publish(string name, string body, string language) {
	if(language == "json") return jsonPublish(name,body);
	else if(language== "xml") return xmlPublish(name, body);
	else return false;
}

string DynamicPage::jsonIndex(string key) {
   stringstream pagej;
   if(Conf::mqtt_service != "" && Conf::mqtt_service != "*")
	   pagej<<"{\""<<Conf::mqtt_domain<<"/"<<Conf::mqtt_subdomain<<"/"<<Conf::mqtt_service<<"\": [";
   else pagej<<"{\""<<Conf::mqtt_domain<<"/"<<Conf::mqtt_subdomain<<"\": [";
   map<string,string>::iterator it;
   std::lock_guard<std::mutex> guard(DPmutex);
   for(it=page.begin(); it!=page.end(); it++) 
   	if(key=="" || match(it->first, key)){
	   if(it!=page.begin()) pagej<<",\n";
	   else pagej <<"\n";
	   pagej<<it->second;
   }
   pagej<<"\n]";
   pagej<<"}"<<endl;
   return pagej.str();
}

string DynamicPage::xmlIndex(string key) {
   stringstream pagej;
   pagej<<"<?xml version=\"1.0\" ?>\n";
   if(Conf::mqtt_service != "" && Conf::mqtt_service != "*")
	   pagej<<"<list name=\""<<Conf::mqtt_domain<<"/"<<Conf::mqtt_subdomain<<"/"<<Conf::mqtt_service<<"\">";
   else pagej<<"<list name=\""<<Conf::mqtt_domain<<"/"<<Conf::mqtt_subdomain<<"\">";
   map<string,string>::iterator it;
   std::lock_guard<std::mutex> guard(DPmutex);
   for(it=xmlpage.begin(); it!=xmlpage.end(); it++) 
   	if(key=="" || match(it->first, key)){
	   pagej <<"\n";
	   pagej<<it->second;
   }
   pagej<<"\n";
   pagej<<"</list>\n";
   return pagej.str();
}

bool DynamicPage::jsonPublish(string name, string item) {
   string uri;
   if(Conf::mqtt_service != "" && Conf::mqtt_service != "*")
	   uri = Conf::mqtt_domain + "/" + Conf::mqtt_subdomain + "/" + Conf::mqtt_service + "/" + name;
   else uri = Conf::mqtt_domain + "/" + Conf::mqtt_subdomain + "/" + name;
   std::lock_guard<std::mutex> guard(DPmutex);
   if(page.count(uri) == 0) page.insert( pair<string, string>(uri, item));
   else page.at(uri) = item;
   return true;
}

bool DynamicPage::xmlPublish(string name, string item) {
   string uri;
   if(Conf::mqtt_service != "" && Conf::mqtt_service != "*")
	   uri = Conf::mqtt_domain + "/" + Conf::mqtt_subdomain + "/" + Conf::mqtt_service + "/" + name;
   else uri = Conf::mqtt_domain + "/" + Conf::mqtt_subdomain + "/" + name;
   std::lock_guard<std::mutex> guard(DPmutex);
   if(xmlpage.count(uri) == 0) xmlpage.insert( pair<string, string>(uri, item));
   else xmlpage.at(uri) = item;
   return true;
}

bool DynamicPage::Delete(string key) {
   bool cont;
   map<string,string>::iterator it = page.begin();
   std::lock_guard<std::mutex> guard(DPmutex);
   while(it != page.end()) {
      cont = true;
      for(it=page.begin(); it!= page.end(); it++)
         if(match(it->first, key)) {
	    page.erase(it);
	    cont = false;;
	 }
   }
   while(it != xmlpage.end()) {
      cont = true;
      for(it=xmlpage.begin(); it!= xmlpage.end(); it++)
         if(match(it->first, key)) {
	    xmlpage.erase(it);
	    cont = false;;
	 }
   }
   return true;
}

bool DynamicPage::match(string uri, string key) {
    if(uri.compare(0, key.length(), key) == 0) return true;
    else return false;
}
