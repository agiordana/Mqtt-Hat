/* 
 * File:   Trigger.h
 * Author: attilio
 *
 * Created on November 21, 2012, 11:19 PM
 */

#ifndef TRIGGER_H
#define	TRIGGER_H

#include "hsrv.h"
#include "Params.h"
//#include <map>
#include <thread>

#define TPRECISION 10
#define NANOPERSECOND 1000000000

class Trigger: public Params {
public:
    Trigger(std::string file);
    
    void exec();
    
    void start();

private:
    std::thread tr;
    
    long Euclide(long, long);
    
    bool initSubject();
    
    double cycle;
    
    std::map<std::string, double> tm;
    
    struct timespec rqtp, rmtp;
};

#endif	/* TRIGGER_H */

