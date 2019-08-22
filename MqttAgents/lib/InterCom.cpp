#include "InterCom.h"

using namespace std;

InterCom::InterCom() {
}

InterCom::InterCom(vector<string>& subject) {
  for(auto i=0; i<subject.size(); i++) 
	  insert ( std::pair<string,MsgQueue<Message>*>(subject[i], new MsgQueue<Message>) );
}

MsgQueue<Message>* InterCom::Attach(const string key) {
   lock_guard<mutex> guard(lock);
   if(count(key) == 0) 
       insert ( std::pair<string,MsgQueue<Message>*>(key, new MsgQueue<Message>) );
   return this->at(key);
}

bool InterCom::Add(const string key) {
   lock_guard<mutex> guard(lock);
   if(count(key) == 0) 
       insert ( std::pair<string,MsgQueue<Message>*>(key, new MsgQueue<Message>) );
   return true;
}

bool InterCom::Connect(const string t, MsgQueue<Message>* q) {
   lock_guard<mutex> guard(lock);
   insert(pair<string,MsgQueue<Message>*>(t, q));
   return true;
}

bool InterCom::Send(const string t, Message& m) {
   if(count(t) >0){
       this->at(t)->send(m);
       return true;
   }
   else return false;
}

Message InterCom::Receive(const string t) {
   if(count(t) >0){
      return this->at(t)->receive();
   }
   else {
      string answer = "Not Found";
      return Message(t,answer);
   }
}
