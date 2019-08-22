/* 
 * File:   Gpio.h
 * Author: attilio
 *
 * Created on May 7, 2015, 10:13 PM
 */

#ifndef GPIO_H
#define	GPIO_H
#include "hsrv.h"
#include "NameList.h"
#include "Params.h"

class Gpio: public Params {
public:
    std::string name;
    std::string gpioname;
    std::string gpiotype;
    Gpio(std::string, std::string);
    std::string pathname;
    std::string gpiovalue;
    double wait_until;
    double w;
    std::string currentValue;
    bool setCurrentValue(std::string);
    bool initialise();
    bool program();
    bool setInitialValue();
    std::string describe(std::string language="json"){
	    if(language=="json") return jsonDescribe();
    	    else return xmlDescribe();
    };
private:
    std::string jsonDescribe();
    std::string xmlDescribe();
    std::string getpath(std::string);
    std::string get_and_set_value(std::string);
    static NameList initDone;
    std::string gpiopath;
};


#endif	/* GPIO_H */
