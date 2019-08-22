/* 
 * File:   ElectricMeter.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef ELECTRICMETER_H
#define	ELECTRICMETER_H
#include "Device.h"
#include "Params.h"

class ElectricMeter: public Params, public Device {
public:
       bool execCmd(std::string& topic, std::string& vv, std::string& tm);
       bool execEvent(std::string& topic, std::string& vv, std::string& tm);
       bool Describe();
       bool sync();
       bool reset();
       bool sendStatus();
       ElectricMeter(std::string, std::string);
private:
        double energy;
        double Ipower;
        double Apower;
        double Wpower;
        double origin;
        double currentTime;
        double alpha;
        double quantum;
        std::string unit;
	double deltaEnergy;
};


#endif	/* ELECTRICMETER_H */

