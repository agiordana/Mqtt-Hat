/*
 *  agent.h
 *  ioagent
 *
 *  Created by Attilio Giordana on 1/5/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

#ifndef _Agent_
#define _Agent_

#include "hsrv.h"
#include "Params.h"
#include "RestHttpServer.h"
#include "MqttClientThread.h"
#include "AgentTask.h"
#include "Logger.h"
#include "AgentKernel.h"
#include "AgentTaskLocal.h"

class Agent: public AgentKernel {
public:
    Agent(std::string a, std::string b);    
};

#endif
