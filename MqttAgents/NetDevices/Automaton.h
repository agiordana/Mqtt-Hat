/*
 * Atomaton.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#ifndef ATOMATON_H_
#define ATOMATON_H_
#include "hsrv.h"
#include "Params.h"
#include "NameList.h"
#include "Component.h"

class Automaton: public Params {
public:
	Automaton(std::string dir, std::string nm);
	bool getObservations(std::vector<std::string>& obs);
	std::string nextState(std::string);
	NameList actions(std::string topic, std::string ty, std::string subty, std::string v, std::string status);
private:
	NameList commands;
	NameList events;
	NameList forbidden;
};


#endif /* ATOMATON_H_ */
