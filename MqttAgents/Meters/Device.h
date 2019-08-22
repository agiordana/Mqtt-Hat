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
#include "Logger.h"
#include "NameList.h"
#include "DynamicPage.h"

class Device {
public:
    std::string name;
    std::string alias;
    std::string type;
    std::string status;
    virtual bool sync();
    bool Connect(std::map<std::string, std::vector<Device*>>*);
    virtual bool execCmd(std::string&, std::string&, std::string&);
    virtual bool execEvent(std::string&, std::string&, std::string&);
    virtual bool Describe();
    virtual bool sendStatus();
    virtual bool reset();
protected:
    std::string in;
    std::string source;
    std::string topic;
};

#endif /* DEVICE_H_ */
