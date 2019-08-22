#ifndef _Logger_
#define _Logger_

#include "Conf.h"
#include "hsrv.h"
#include "Params.h"
#include "InterCom.h"

class Logger {
public:
   Logger(std::string f);
   static bool log(std::string localname, std::string message);
   static bool status(std::string localname, std::string message);
   static bool notify(std::string topic, std::string infotype, std::string message);
   static bool describe(std::string topic, std::string message);
private:
   static std::string log_theme;
   static std::string status_theme;
   static std::string notify_theme;
   static std::string describe_theme;
   static std::string log_msg;
   static std::string status_msg;
   static std::string notify_msg;
   static std::string describe_msg;
};

#endif
