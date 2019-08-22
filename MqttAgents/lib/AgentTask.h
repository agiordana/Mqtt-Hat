#ifndef _AgentTask_
#define _AgentTask_

#include <thread>
#include "Conf.h"
#include "hsrv.h"
#include "InterCom.h"
#include "Logger.h"

class AgentTask {
public:
   AgentTask(std::string);
   void start();
   std::string id;
private:
   std::thread ep;
   std::thread ce;
   virtual void EventProcessor(std::string);
   virtual void CmdExecutor(std::string);
};

#endif

