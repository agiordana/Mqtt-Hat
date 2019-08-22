/* 
 * File:   DigitalInput.h
 * Author: attilio
 *
 * Created on May 12, 2015, 2:58 PM
 */

#ifndef _ANALOGINPUT_H
#define	_ANALOGINPUT_H
#include "Device.h"
#include "Params.h"
#include <math.h>
#define SEPARATOR '$'

class AnalogMeter: public Params, public Device {
public:
       bool execCmd(std::string& topic, std::string& vv, std::string& tm);
       bool execEvent(std::string& topic, std::string& vv, std::string& tm);
       bool Describe();
       bool sync();
       bool reset();
       bool sendStatus();
       AnalogMeter(std::string, std::string);
private:
	double scale_factor;
        double currentTime;
	std::string currentValue;
	std::string unit;
        std::string remap(std::string&);
	std::string probe;
	double tprobe_v12(double&, double&);
};



#endif	/* BINARYINPUT_H */

