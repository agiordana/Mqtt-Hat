/*
 *  agent.cpp
 *  iopwragent
 *
 *  Created by Davide Monfrecola on 07/11/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "AgentKernel.h"

AgentKernel::AgentKernel(string path0, string path1) {
    load_conf(path0, path1);
    logger = new Logger("logger");
    Conf::mbx.Add("publisher");
    Conf::mbx.Add("cmdexecutor");
    mqtt = new MqttClientThread("MqttClientThread");
    activity = new AgentTask("AgentTask");
    trigger = new Trigger("trigger");
}

void AgentKernel::start() {
    mqtt->start();
    activity->start();
    trigger->start();
    Logger::log("Agent", "Started");
    if (!start_rest_server()) {
      exit(-1);
    }
}

bool AgentKernel::start_rest_server() {
   rest_server = new RestService();
   rest_server->start();
   return true;
}

bool AgentKernel::load_conf(string& path0, string& path1) {
    string hname = hsrv::getHostName();
    Conf::configdir = path1;
    Conf::bindir = FileManager::extractBin(string(path0));
    if (!FileManager::isDir(Conf::configdir)) {
        return false;
    }
    Params global("conf");
    global.load();
    Conf::homedir = global.get("homedir");
    string roothomedir = FileManager::getRoot(Conf::homedir);
    if(!FileManager::isDir(roothomedir))
	Conf::homedir = global.get("emergency_homedir");
    Conf::agenttype = global.get("agenttype");
    Conf::agentname = global.get("agentname");
    hsrv::strReplace(Conf::agentname, "$HOST", hname);
    Conf::behaviour = global.get("behaviour");
    Conf::mqtt_domain = global.get("mqtt_domain");
    if(Conf::mqtt_domain == "") Conf::mqtt_domain = "local";
    Conf::mqtt_subdomain = global.get("mqtt_subdomain");
    if(Conf::mqtt_subdomain == "") Conf::mqtt_subdomain = hname;
    Conf::mqtt_service = global.get("mqtt_service");
    if (Conf::mqtt_service == "") Conf::mqtt_service = "*";
    if (Conf::behaviour == "") Conf::behaviour = "*";
    Conf::port = global.get("port");
    if(Conf::port == "") Conf::port = "8082";
    //cout<<"Domain: "<<Conf::mqtt_domain<<", Subdomain: "<<Conf::mqtt_subdomain<<", Service: "<<mqtt_service<<endl;
    return true;
}
