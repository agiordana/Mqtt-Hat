/*
 * Device.h
 *
 *  Created on: Jun 23, 2017
 *      Author: Attilio
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "hsrv.h"
#include "Conf.h"
#include "Component.h"
#include "Automaton.h"
#include "Logger.h"
#include "NameList.h"
#include "DynamicPage.h"

class Device: public Params {
public:
    std::string name;
    Device(std::string dir, std::string nm);
    bool sync(std::string&, std::string&, std::string&);
    bool Connect(std::map<std::string, std::vector<Device*>>*);
    bool execCmd(std::string&, std::string&, std::string&);
    bool execEvent(std::string&, std::string&, std::string&);
    bool Describe();
    bool sendStatus();

protected:
	unsigned cycle;
	unsigned turn;
	unsigned scount;
	std::vector<Component> component;
	Automaton* control;
	void do_work(Device* obj);
	bool load(std::string& dir, std::string& nm);
	std::string status();
	std::string status(std::string);
	std::string aggregateValue(std::string);
        std::string valueNormalize(std::string value);
        std::string statusAligne(std::string s, std::string devtype);
        std::string setValue(std::string s);
        bool getObservations(std::vector<std::string>&);
};

#endif /* DEVICE_H_ */
