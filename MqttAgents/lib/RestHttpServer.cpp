/* 
 * File:   RestHttpServer.h
 * Author: Attilio Giordana
 *
 * Created on 18 novembre 2014, 9.44
 */


/*
     This file is part of libhttpserver
     Copyright (C) 2014 Sebastiano Merlino

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
     USA
 */


#include "RestHttpServer.h"

bool verbose = false;

using namespace httpserver;
using namespace std;

std::mutex RHSmutex;

bool RestService::setPort() {
   unsigned p;
   string port = Conf::port;
   sscanf(port.c_str(),"%us", &p);
   s_port = (unsigned short)p;
   return true;
}
	

const http_response RestService::render_GET(const http_request &req) {
    int resp_code;
    string resp_body;
    string resp_type;
    string uri;
    if (req.get_path() == "" or req.get_path() == "/") {
      uri = "/index.html";      
    } else {
      uri = req.get_path(); 
    }
    if(uri[0]!='/') uri = "/"+uri;
    uri = Conf::configdir+"/doc"+uri;
    std::lock_guard<std::mutex> guard(RHSmutex);
    if(FileManager::getStem(FileManager::getTail(uri)) == "DynamicPage") {
	    string language = FileManager::getExt(FileManager::getTail(uri));
	    resp_body = DynamicPage::Index("",language);
	    resp_code = 202;
	    if(language=="json") resp_type = "application/json";
	    else resp_type = "application/xml";
    }
    else if(!FileManager::isFile(uri)) {
	resp_body = "Not Found";
	resp_code = 404;
	resp_type = "text";
    }
    else {
	resp_body = FileManager::readFile(uri);
	resp_code = 200;
	resp_type = "text/html";
    }
    http_response res(http_response_builder(resp_body, resp_code, resp_type).string_response());
    return res;
}


const http_response RestService::render_PUT(const http_request &req) {
    int resp_code;
    string resp_body;
    string resp_type;
    string body;
    string uri;
    
    uri = req.get_path();
    if(uri=="/" || uri=="") uri="/index.html";
    if(uri[0]!='/') uri = "/"+uri;
    uri = Conf::configdir+"/doc"+uri;
  cout<<"PUT: "<<uri<<endl;
    body = req.get_content();
    std::lock_guard<std::mutex> guard(RHSmutex);
    if(FileManager::saveFile(uri,body)) {
       resp_code = 200;
       resp_body = "done";
    }
    else {
       resp_code = 303;
       resp_body = "Failed";
    }
    
    http_response res(http_response_builder(resp_body, resp_code, resp_type).string_response());
    return res;
}


const http_response RestService::render_POST(const http_request &req) {
    int resp_code;
    string resp_body;
    string resp_type;
    string body;
    string uri;
    uri = req.get_path();
    if(uri=="/" || uri=="") uri="/index.html";
    if(uri[0]!='/') uri = "/"+uri;
    uri = Conf::configdir+"/doc"+uri;
  cout<<"POST: "<<uri<<endl;
    body = req.get_content();
    std::lock_guard<std::mutex> guard(RHSmutex);
    if(FileManager::saveFile(uri,body)) {
       resp_code = 200;
       resp_body = "done";
    }
    else {
       resp_code = 303;
       resp_body = "Failed";
    }
    http_response res(http_response_builder(resp_body, resp_code, resp_type).string_response());
    return res;
}


const http_response RestService::render(const http_request &req) {

    if (verbose) std::cout << req;
//  cout<<req<<endl;
    http_response res(http_response_builder("generic response", 200).string_response());

    if (verbose) std::cout << res;
    return res;
}

const http_response RestService::render_HEAD(const http_request &req) {

    if (verbose) std::cout << req;
//  cout<<req<<endl;

    http_response res(http_response_builder("HEAD response", 200).string_response());

    if (verbose) std::cout << res;
    return res;
}

const http_response RestService::render_OPTIONS(const http_request &req) {

    if (verbose) std::cout << req;
//  cout<<req<<endl;

    http_response res(http_response_builder("OPTIONS response", 200).string_response());

    if (verbose) std::cout << res;
    return res;
}

const http_response RestService::render_CONNECT(const http_request &req) {

    if (verbose) std::cout << req;
//  cout<<req<<endl;

    http_response res(http_response_builder("CONNECT response", 200).string_response());

    if (verbose) std::cout << res;
    return res;
}

const http_response RestService::render_DELETE(const http_request &req) {
 int resp_code;
    string resp_body;
    string resp_type;
    string uri;
    uri = req.get_path(); 
    if(uri == "" ||uri=="/") {
       resp_body = "Forbidden";
       resp_code = 403;
    } else {
       if(uri[0] != '/') uri = "/"+uri;
       uri = Conf::configdir+"/doc" + uri;
       string filename = FileManager::getTail(uri);
       string dir = FileManager::getRoot(uri);
       cout<<"DELETE: "<<dir<<" "<<filename<<endl;
       std::lock_guard<std::mutex> guard(RHSmutex);
       if(FileManager::deleteFile(dir, filename)) {
	  resp_body = "done";
          resp_code = 200;
       } else {
	  resp_body = "Failed";
	  resp_code = 303;
       }
    }
    
    http_response res(http_response_builder(resp_body, resp_code).string_response());
    return res;
}


bool RestService::start(std::string n) {
    setPort();
    name = n;
    webserver ws = create_webserver(s_port).start_method(http::http_utils::INTERNAL_SELECT).max_threads(5);
    ws.register_resource(name, this, true);
    ws.start(true);
    return 0;
}

