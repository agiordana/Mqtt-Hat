#ifndef _RULESET_H
#define _RULESET_H

#include "Rule.h"
#include "Precondition.h"

class RuleSet:: public std::map<std::string, Rule*> {
public:
   bool Load(std::string dir);
   bool Reload(std::string dir);
   std::string describe(std::string lang="json");
   bool connect(InputHub&, InputHub&);
   // match?????
private:
   std::vector<Rule*> rr;
};

#endif
