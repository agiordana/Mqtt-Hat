//
//  MqttClientThread.h
//  logger
//
//  Created by Attilio Giordana on 24/02/18
//

#ifndef MQTT_CLIENT_THREAD
#define MQTT_CLIENT_THREAD
#include "Params.h"
#include "Conf.h"
#include "MqttClient.h"
#include <thread>

class MqttClientThread {
public:
    MqttClientThread(std::string n);
    std::string id;
    void start();
    static MqttClient* mqttclient;
    
protected:
    
    std::thread publisher;
    std::thread controller;
    static void do_work();
    static void do_work1();
    std::string topicComplete(std::string&);
};

#endif
