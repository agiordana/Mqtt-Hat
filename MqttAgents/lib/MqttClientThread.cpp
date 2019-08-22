//
//  MqttClientThread.cpp
//  logger
//
//  Created by Attilio Giordana on 8/27/14.
//  Copyright (c) 2014 Università del Piemonte Orientale. All rights reserved.
//

#include "MqttClientThread.h"

using namespace std;

MqttClient* MqttClientThread::mqttclient = NULL;

MqttClientThread::MqttClientThread(string n) {
    id = n;
    vector<string> subjects;
    string threadpath = "threads/"+id;
    Params params(threadpath);
    params.load();
    string host = params.get("host");
    string asciiport = params.get("port");
    if(host == "") host = "localhost";
    if(asciiport == "") asciiport = "1883";
    int port = hsrv::a2int(asciiport);
    setof<Attribute> topic = params.extractCategory("topic");
    for(size_t i=0; i<topic.size(); i++) {
       if(topic[i].value=="mqtt_sub") subjects.push_back(topicComplete(topic[i].name));
    }
    //for(auto i=0; i<subjects.size(); i++) cout<<subjects[i]<<endl;

    string client="client"+hsrv::double2a(hsrv::gettime());
    //cout<<"Registering as: "<<client<<endl;
    mosqpp::lib_init();
    
    mqttclient = new MqttClient(client.c_str(), host.c_str(), port, subjects);
}

void MqttClientThread::start() {
    publisher = thread(do_work);
    controller = thread(do_work1);
}

void MqttClientThread::do_work() {
    MsgQueue<Message>* input = Conf::mbx.Attach("publisher");
    Message msg;
    while (true) {
        msg = input->receive();
    	string pub_topic = msg.theme;
	string message = msg.content;
        mqttclient->publish(pub_topic, message);
    }
}

void MqttClientThread::do_work1() {
  int rc;
  while(true){
           rc = mqttclient->loop();
           if(rc){
                 mqttclient->reconnect();
           }
        }
}

string MqttClientThread::topicComplete(string& proto) {
   string lproto = proto;
   hsrv::strReplace(lproto, "$MQTT_DOMAIN", Conf::mqtt_domain);
   hsrv::strReplace(lproto, "$MQTT_SUBDOMAIN", Conf::mqtt_subdomain);
   if(Conf::mqtt_service != "" && Conf::mqtt_service != "*")
       hsrv::strReplace(lproto, "$MQTT_SERVICE", Conf::mqtt_service);
   return lproto;
}

