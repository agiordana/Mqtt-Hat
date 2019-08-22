#ifndef _RuleHub_
#define _RuleHub_

#include <map>
#include "Rule.h"
#include "DynamicPage.h"

class Input;
class Condition;
class Event;
class Rule;

class RuleHub: public std::map<std::string, std::vector<Input*>> {
	friend class Rule;
public:
   bool Load(std::string);
   bool Reload(std::string);
   bool print();
   bool timeUpdate();
private:
   std::vector<Input*> condition;
   std::vector<Input*> event;
   std::vector<Rule*> rule;
};

#endif
