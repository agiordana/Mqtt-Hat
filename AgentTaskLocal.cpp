#include "AgentTaskLocal.h"

using namespace std;

vector<Gpio*> AgentTaskLocal::input;
vector<Gpio*> AgentTaskLocal::output;

void AgentTaskLocal::EventProcessor(string id) {
   int i=0;
//   vector<Gpio*> input;
   struct pollfd* pfd;
   Logger::log("EventProcessorLocal","Started");
   Load("gpioin", input);
   pfd = (struct pollfd*)malloc(sizeof(struct pollfd)*input.size());
   int value;
   Message out_mess;
   for(auto i=0; i<input.size(); i++) {
       pfd[i].fd = open(input[i]->gpiovalue.c_str(), O_RDONLY);
       pfd[i].events = POLLPRI;
       pfd[i].revents = 0;
   }
   for(auto i=0; i<input.size(); i++) {
       value = get_current_value(pfd[i].fd);
       input[i]->currentValue = hsrv::int2a(value);
       DynamicPage::Publish(input[i]->name, input[i]->describe("json"),"json");
       DynamicPage::Publish(input[i]->name, input[i]->describe("xml"),"xml");
   }

   while(true) {
      int ready = poll(pfd, (int)input.size(), -1);
      for(size_t i=0; i<input.size(); i++)
          if(pfd[i].revents != 0) {
               value = get_current_value(pfd[i].fd);
               string asciivalue = hsrv::int2a(value);
	       double tt = hsrv::gettime();
	       if(input[i]->wait_until <= tt && asciivalue != input[i]->currentValue) {
	          input[i]->currentValue = asciivalue;
		  DynamicPage::Publish(input[i]->name, input[i]->describe("json"),"json");
		  DynamicPage::Publish(input[i]->name, input[i]->describe("xml"),"xml");
	          if(input[i]->wait_until>0) Logger::notify(input[i]->get("event"), "\"event\"", asciivalue);
	          input[i]->wait_until = tt + input[i]->w;
	       }
	  }
   }
}


void AgentTaskLocal::CmdExecutor(string id) {
  Logger::log("CmdExecutorLocal", "Started");
//  vector<Gpio*> output;
  Load("gpioout", output);
  size_t n = 0;
  MsgQueue<Message>* cmdsource = Conf::mbx.Attach("cmdexecutor");
  while(true) {
       Message cmd = cmdsource->receive();
       string name;
       string value;
       string todo;
       decode(cmd, todo, name, value);
       if(todo == "request") {
	       if(value=="status") sendStatus(output);
	       else if(value=="status_tr") {
		       sendStatusTR(output,n++);
		       if(n>=output.size()) n=0;
	       }
	       else if(value=="description"||value=="description.json") sendDescription("json");
	       else if(value=="description.xml") sendDescription("xml");
       }
       else if(todo=="cmd") {

          for(auto i=0; i<output.size(); i++)
	       if(name == output[i]->name) {
		       output[i]->setCurrentValue(value);
   		       Logger::notify(output[i]->get("event"), "\"event\"", output[i]->currentValue);
       		       DynamicPage::Publish(output[i]->name, output[i]->describe("json"),"json");
       		       DynamicPage::Publish(output[i]->name, output[i]->describe("xml"),"xml");
		       break;
	       }
       }
  }
}

bool AgentTaskLocal::Load(string file, vector<Gpio*>& dest) {
  string gpiodir = Conf::configdir+"/"+file;
  if(!FileManager::isDir(gpiodir)) return false;
  vector<string> filelist;
  FileManager::fileList(gpiodir,filelist);
  for(auto i=0; i<filelist.size(); i++) {
      dest.push_back(new Gpio(file, FileManager::getStem(filelist[i])));
  }
  for(auto i=0; i<dest.size(); i++) dest[i]->program();
  registerDevices(dest);
  
  return true;
}

bool AgentTaskLocal::registerDevices(vector<Gpio*>& gpio) {
  for(auto i=0; i<gpio.size(); i++) {
	  DynamicPage::Publish(gpio[i]->name, gpio[i]->describe("json"),"json");
	  DynamicPage::Publish(gpio[i]->name, gpio[i]->describe("xml"),"xml");
  }
  return true;
}

int AgentTaskLocal::get_current_value(int fd) {
    int value;
    lseek(fd, 0, 0);

    char buffer[1024];
    int size = read(fd, buffer, sizeof(buffer)-1);
    if (size != -1) {
         buffer[size] = '\0';
         value = atoi(buffer);
    }
    else {
         value = -1;
    }
    return value;
}

bool AgentTaskLocal::decode(Message& msg, string& todo, string& name, string& value) {
   NameList todo_value;
   todo_value.init(msg.content, ':');
   if(todo_value.size()!=2) {
      Logger::log("\"MessageDecode_Error\"", msg.content);
      return false;
   }
   name = FileManager::getTail(msg.theme);
   todo = value = "";
   for(auto i=0; i<todo_value[0].length(); i++)
      if(todo_value[0][i]!='{' && todo_value[0][i]!='}' && todo_value[0][i]!=' ') todo += todo_value[0][i]; 
   for(auto i=0; i<todo_value[1].length(); i++)
      if(todo_value[1][i]!='{' && todo_value[1][i]!='}' && todo_value[1][i]!=' ') value += todo_value[1][i]; 
   return true;
}


bool AgentTaskLocal::sendStatus(vector<Gpio*>& output) {
   for(size_t i=0; i<output.size(); i++) Logger::notify(output[i]->get("event"), "\"status\"", output[i]->currentValue);
   return true;
}

bool AgentTaskLocal::sendStatusTR(vector<Gpio*>& output, size_t i) {
	if(i>output.size()) return false;
	Logger::notify(output[i]->get("event"), "\"status\"", output[i]->currentValue);
	return true;
}

bool AgentTaskLocal::sendDescription(string language) {
	Logger::describe("description",DynamicPage::Index("",language));
	return true;
}
