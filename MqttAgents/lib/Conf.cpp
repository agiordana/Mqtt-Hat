//
//  Conf.cpp
//

using namespace std;

#include "Conf.h"

string Conf::configdir = "";
string Conf::agenttype = "*";
string Conf::agentname = "*";
string Conf::homedir = "";
string Conf::behaviour = "*";
string Conf::mqtt = "OFF";
string Conf::mqtt_domain = "*";
string Conf::mqtt_subdomain = "*";
string Conf::mqtt_service = "*";
string Conf::logfile = "LOG";
string Conf::bindir = "";
string Conf::port = "8082";
InterCom Conf::mbx;
