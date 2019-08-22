#ifndef _InterCom_
#define _InterCom_

#include <map>
#include "Message.h"
#include <mutex>
#include "Template.h"

class InterCom: public std::map<std::string, MsgQueue<Message>*> {
public:
   InterCom();
   InterCom(std::vector<std::string>&);
   MsgQueue<Message>* Attach(const std::string);
   bool Add(const std::string);
   bool Connect(const std::string, MsgQueue<Message>*);
   bool Send(const std::string, Message&);
   Message Receive(const std::string);
private:
   std::mutex lock;
};

#endif
