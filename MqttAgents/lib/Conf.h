//
//  hsrv.h
//  phoneagent
//
//  Created by Davide Monfrecola on 11/15/11.
//  Copyright 2011 Università del Piemonte Orientale. All rights reserved.
//


#ifndef _CONF_H_
#define _CONF_H_

#include "hsrv.h"
#include "InterCom.h"

class Conf {
public:
   static std::string configdir;
   static std::string homedir;
   static std::string bindir;
   static std::string logfile;
   static std::string agenttype;
   static std::string agentname;
   static std::string behaviour;	
   static std::string port;	
   static std::string mqtt;	
   static std::string mqtt_domain;	
   static std::string mqtt_subdomain;	
   static std::string mqtt_service;	
   static InterCom mbx;	
   //static bool load(std::string ar0, std::string ar1);
};


#endif
