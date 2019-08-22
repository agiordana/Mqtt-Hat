#ifndef _AgentTaskLocal_
#define _AgentTaskLocal_

#include "DeviceHub.h"
#include "DynamicPage.h"
#include "AgentTask.h"
#include "Message.h"
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
	static DeviceHub device;
	bool Configure(std::string&);
	bool Delete(std::string&);
	bool mkManifest(std::string&, std::string&);
	bool mkControl(std::string&, std::string&, std::string&, std::string&, unsigned);
	std::string mkAutomaton(std::string&, std::string&, unsigned);
	std::string mkAutomatonUpDown(std::string&, std::string&);
	std::string mkTrigger(std::string&, std::string&);
	bool mkComponent(std::string&, std::string&, std::string&, unsigned);
	bool generateGrayCode(int, std::vector<std::string>&);
	std::string cleanString(std::string&);
};

#endif
