#ifndef _AgentTaskLocal_
#define _AgentTaskLocal_

#include "DynamicPage.h"
#include "AgentTask.h"
#include "Message.h"
#include "RuleHub.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <math.h>

class AgentTaskLocal: public AgentTask {
public:
	AgentTaskLocal(std::string id): AgentTask(id) {};
	void CmdExecutor(std::string) override;
	void EventProcessor(std::string) override;
private:
	bool decode(Message&, std::string&, std::string&, std::string&);
	bool sendDescription(std::string);
	static RuleHub rule;
	bool Configure(std::string&);
	bool Delete(std::string&);
	bool mkManifest(std::string&, std::string&);
	std::string cleanString(std::string&);
};

#endif
