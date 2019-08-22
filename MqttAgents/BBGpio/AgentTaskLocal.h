#ifndef _AgentTaskLocal_
#define _AgentTaskLocal_

#include "Gpio.h"
#include "DynamicPage.h"
#include "AgentTask.h"

class AgentTaskLocal: public AgentTask {
public:
	AgentTaskLocal(std::string id): AgentTask(id) {};
	void EventProcessor(std::string) override;
	void CmdExecutor(std::string) override;
private:
	static std::vector<Gpio*> input;
	static std::vector<Gpio*> output;
	bool Load(std::string, std::vector<Gpio*>&);
	bool registerDevices(std::vector<Gpio*>&);
	int get_current_value(int);
	bool decode(Message&, std::string&, std::string&, std::string&);
	bool sendStatus(std::vector<Gpio*>&);
	bool sendStatusTR(std::vector<Gpio*>&, std::size_t);
	bool sendDescription(std::string);
};

#endif
