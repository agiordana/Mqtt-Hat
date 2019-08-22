#ifndef _Message_
#define _Message_

#include "hsrv.h"

class Message {
public:
   std::string theme;
   std::string content;
   std::string tm;
   bool print(){ std::cout<<"Theme: "<<theme<<", Content: "<<content<<", Time: "<<tm<<std::endl;};
   Message() {theme=content=tm="";priority=0;};
   Message(std::string t, std::string c) {
	   theme = t; content = c; tm = hsrv::double2a(hsrv::gettime());
	   priority = 0;
   };
   int priority;
};

#endif
