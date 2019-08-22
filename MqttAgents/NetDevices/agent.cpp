/*
 *  agent.cpp
 *  iopwragent
 *
 *  Created by Davide Monfrecola on 07/11/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "agent.h"

Agent::Agent(string path0, string path1):AgentKernel::AgentKernel(path0, path1) {
   activity = new AgentTaskLocal("AgentTask");
   start();
}

