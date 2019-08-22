#include "AgentTask.h"

using namespace std;

AgentTask::AgentTask(string name) {
   id = name;
}

void AgentTask::start() {
  ep = thread(&AgentTask::EventProcessor,this, id);
  ce = thread(&AgentTask::CmdExecutor,this, id);
}

void AgentTask::CmdExecutor(string id) {
	Logger::log("CmdExecutor", "Started");
   MsgQueue<Message>* cmdsource = Conf::mbx.Attach("cmdexecutor");
   while(true) {
      Message cmd = cmdsource->receive();
      cmd.print();
   }
}

void AgentTask::EventProcessor(string id) {
   int i=0;
   Logger::log("EventProcessor","Started");
   while(true) {
      string message = hsrv::int2a(i++);
      Logger::notify("event", "\"GPIO\"", message);
      sleep(2);
   }
}
