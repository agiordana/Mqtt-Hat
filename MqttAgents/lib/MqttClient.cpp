#include <cstdio>
#include <cstring>

#include "MqttClient.h"
#include <mosquittopp.h>
#define MAXBUF 16000

using namespace std;

MqttClient::MqttClient(const char *id, const char *host, int port, vector<string>& subjects) : mosquittopp(id) {
	int keepalive = 600;

	/* Connect immediately. This could also be done by calling
	 * MqttClient->connect(). */
	for(size_t i=0; i<subjects.size(); i++) tosubscribe.push_back(subjects[i]);
//	for(size_t i=0; i<tosubscribe.size(); i++) cout<<tosubscribe[i]<<endl;
	connect(host, port, keepalive);
};

MqttClient::~MqttClient() { }

void MqttClient::on_connect(int rc) {
//	printf("Connected with code %d.\n", rc);
        if(rc == 0) 
	   for(size_t i=0; i<tosubscribe.size(); i++) {
	       subscribe(NULL, tosubscribe[i].c_str());
	   }
}

void MqttClient::on_message(const struct mosquitto_message *message) {

//	printf("topic: %s\n", message->topic);
//	printf("%s\n", message->payload);	
   char buf[MAXBUF];
   sprintf(buf,"%s", message->payload);
   string topic = message->topic;
   string mm = buf;
   sendCmd(topic, mm);
}

void MqttClient::on_subscribe(int mid, int qos_count, const int *granted_qos) {
//	printf("Subscription succeeded.\n");
}

bool MqttClient::publish(string& topic, string& message) {
   int res;
   int ln = (int) message.size();
   mosqpp::mosquittopp::publish(NULL, topic.c_str(), ln, message.c_str());
   res = loop();
   while(res) reconnect();
//   printf("res %d\n", res);
   if(res == 0) return true;
   else return false;
}

bool MqttClient::sendCmd(string topic, string message) {
   Message msg(topic,message);
   Conf::mbx.Send("cmdexecutor", msg);
}

bool MqttClient::isTag(string& name) {
  if(name.length() >2 && name[0] == 't' && name[1] == '.') return true;
  else return false;
}
