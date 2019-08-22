#ifndef _DynamicPage_
#define _DynamicPage_

#include "hsrv.h"
#include "Conf.h"
#include <map>
#include <sstream>
#include <mutex>

class DynamicPage {
public:
	static std::string Index(std::string key, std::string language = "json");
	static bool Publish(std::string name, std::string body, std::string language="json");
	static bool Delete(std::string);
	static bool Clear() {page.clear(); xmlpage.clear(); return true;}
private:
	static std::map<std::string, std::string> page;
	static std::map<std::string, std::string> xmlpage;
	static bool match(std::string, std::string);
	static std::mutex DPmutex;
	static std::string jsonIndex(std::string key);
	static std::string xmlIndex(std::string key);
	static bool xmlPublish(std::string, std::string);
	static bool jsonPublish(std::string, std::string);
};

#endif
