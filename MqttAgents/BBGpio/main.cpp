using namespace std;

#include "agent.h"
#include <sys/wait.h>

Agent* agent;

int main (int argc, char * const argv[]) {
    	int status = 0;
	pid_t process;
	if(argc < 2) {
          cout << "Usage: soapagent <confdir>" << endl;
          return 0;
    	}

	if(setreuid(geteuid(),geteuid())<0) cout<<"Failed to become root!!!"<<endl;
	if(setregid(getegid(),getegid())<0) cout<<"Failed to become staff!!!"<<endl;
	while(true) {
	     if((process = fork()) == 0) {
    
		agent = new Agent(string(argv[0]),string(argv[1]));
    
      	     }
      	     else waitpid(-1,&status,0);
	     if(status != 256 ) sleep(20);
     	}
	return 0;
}
