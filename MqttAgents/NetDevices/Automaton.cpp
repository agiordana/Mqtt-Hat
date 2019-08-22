/*
 * Automaton.cpp
 *
 *  Created on: Jun 25, 2017
 *      Author: Attilio
 */

#include "Automaton.h"

using namespace std;

Automaton::Automaton(string dir, string nm): Params::Params(dir, nm) {
	load();
	commands.init(get("cmd", "trigger"),'+');
	events.init(get("event", "trigger"),'+');
	if(get("forbidden", "trigger")!="") forbidden.init(get("forbidden", "trigger"), '+');
}

string Automaton::nextState(string currentState) {
	return get(currentState,"control");
}

bool Automaton::getObservations(vector<string>& obs) {
	NameList cmds;
	NameList events;
	cmds.init(get("cmd", "trigger"), '+');
	events.init(get("event", "trigger"), '+');
	for(size_t i=0; i<events.size(); i++) obs.push_back(events[i]+"_event_execEvent");
	for(size_t i=0; i<cmds.size(); i++) obs.push_back(cmds[i]+"_cmd_execCmd");
	return true;
}

NameList Automaton::actions(string topic, string ty, string devtype, string v, string status) {
	NameList al;
	NameList trigger;;
	if(ty == "cmd" && devtype == "honoff") {
	   if(forbidden.member(v)) return al;
	   al.push_back(v);
	   return al;
	}
	else if(ty == "cmd" && devtype != "honoff") {
	   if(!commands.member(topic)) return al;
	   if(forbidden.member(v)) return al;
	   al.init(v,'+');
	}
	else if(ty == "event") {
	   if(!events.member(topic)) return al;
	   if(get(topic,"input") != v) return al;
	   string value = get(topic,"remap");
	   al.init(get(status,"control"),'+');
	}
	return al;
}

