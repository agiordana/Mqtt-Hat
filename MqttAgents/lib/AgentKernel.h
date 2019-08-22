/*
 *  agent.h
 *  ioagent
 *
 *  Created by Attilio Giordana on 1/5/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _AgentKernel_
#define _AgentKernel_

#include "hsrv.h"
#include "Params.h"
#include "RestHttpServer.h"
#include "MqttClientThread.h"
#include "AgentTask.h"
#include "Logger.h"
#include "Trigger.h"

class AgentKernel {
public:
    AgentKernel(std::string, std::string);    
    void start();
protected:
    Logger* logger;
    RestService* rest_server;
    MqttClientThread* mqtt;
    AgentTask* activity;
    Trigger* trigger;
    bool start_rest_server();
    bool load_conf(std::string&, std::string&);
};

#endif
