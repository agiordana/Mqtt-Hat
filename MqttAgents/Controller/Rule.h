#ifndef _RULE_H
#define _RULE_H
#include "RuleHub.h"
#include "NameList.h"
#include "Logger.h"
#include "hsrv.h"
#include <math.h>

class Rule;
class RuleHub;
class Behavior;
class MonostableBehavior;
class BistableBehavior;

class Input {
public:
   std::string name;
   std::string alias;
   Input(std::string tp, std::string df);
   double value;
   bool match(double);
   double reset();
   std::string description(std::string lang = "json");
   bool getTopic(std::vector<std::string>&);
   virtual bool connect(std::map<std::string,std::vector<Input*>>*);
   virtual bool update(std::string&);
   virtual bool checkDeadline();
   virtual bool isActive();
   Rule* getDependingRule() {return rule;};
   std::string status;
   bool print();
   std::string Describe(std::string language="json");
protected:
   double defvalue;
   double deadline;
   double duration;
   double updtime;
   Rule* rule;
   std::string topic;
};

class Condition: public Input {
public:
   Condition(Rule* r, std::string tp, std::string dval);
   bool connect(std::map<std::string,std::vector<Input*>>*);
   bool update(std::string&);
   bool isActive();
};

class Event: public Input {
public:
   Event(Rule* r, std::string tp, std::string dval, std::string);
   bool connect(std::map<std::string,std::vector<Input*>>*);
   bool update(std::string&);
   bool checkDeadline();
   bool isActive();
};


class Rule: public Params {
public:
    friend Behavior;
    friend MonostableBehavior;
    friend BistableBehavior;
    std::string name;
    Rule(RuleHub*, std::string, std::string);
    bool isActive();
    std::string describe(std::string lang="json");
    bool print();
    bool update(std::string val);
    bool update();
    std::string ty;
    bool Describe();
protected:
    std::vector<Condition*> tcond;
    std::vector<Condition*> fcond;
    std::vector<Event*> ev;
    Behavior* behavior;
    double ctime;
    std::string status;
};

class Behavior {
public:
    virtual bool update(std::string val);
    virtual bool update();
    bool isConditionActive();
    bool active;
    std::string Describe(std::string language="json");
protected:
    bool sendAction(std::string);
    double ctime;
    Rule* r;
    std::string action_on;
    std::string action_off;
    NameList target;
    bool isTimeEnabled();
    double from;
    double to;
};

class MonostableBehavior: public Behavior {
public:
    MonostableBehavior(Rule*);
    bool update(std::string) override;
    bool update() override;
private:
    double waituntil;
    double duration;
    int minev;
};

class BistableBehavior: public Behavior {
public:
    BistableBehavior(Rule*);
    bool update(std::string) override;
    bool update() override;
private:
    double waituntil;
    double repeat_every;
    std::string action_active;
};

#endif
