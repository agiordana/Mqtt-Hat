/* 
 * File:   RestHttpServer.cpp
 * Author: andrea
 * 
     * Created on 18 novembre 2014, 9.44
 */

#ifndef RESTHTTPSERVER_H
#define RESTHTTPSERVER_H

#include <httpserver.hpp>
#include "Conf.h"
#include "DynamicPage.h"
#include "FileManager.h"
#include <thread>
#include <mutex>


using namespace httpserver;

class RestService : public http_resource {
public:
    
    RestService() {};

    ~RestService() {
    };

    bool setPort();
	
    bool start(std::string n = "");

    const http_response render_GET(const http_request &req);

    const http_response render_PUT(const http_request &req);

    const http_response render_POST(const http_request &req);

    const http_response render(const http_request &req);

    const http_response render_HEAD(const http_request &req);

    const http_response render_OPTIONS(const http_request &req);

    const http_response render_CONNECT(const http_request &req);

    const http_response render_DELETE(const http_request &req);

private:
    std::string name;
    uint16_t s_port;
};


#endif
