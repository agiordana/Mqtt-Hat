/*
 * Component.h
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_
#include "hsrv.h"
#include "Params.h"
#include "Logger.h"

class Component: public Params {
public:
	Component(std::string,std::string);
	std::string name();
	bool on();
	bool off();
	std::string setValue(std::string);
	std::string status();
	std::string active();
	std::string setActive(std::string);
	std::string statusDescr();
	bool sync(std::string&, std::string&, std::string&);
	bool getObservations(std::vector<std::string>& obs);
	bool getSubjects(Params& subj);
	std::string Describe(std::string);
private:
	bool check();
	std::string lstatus;
	std::string lmode;
	double last_update;
	std::string getValue(std::string&, std::string);
};

#endif /* COMPONENT_H_ */
