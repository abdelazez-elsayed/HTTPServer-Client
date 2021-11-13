//
// Created by zezo on 11/7/2021.
//

#include "HttpBuilder.h"
#include <fstream>
#include<sstream>
#include <direct.h>
#include <iostream>



HttpBuilder::HttpBuilder() {
 body = nullptr;
}

string HttpBuilder::to_string() {


    stringstream ss;
    ss << "HTTP/1.1" << " " << status_code << " " << reason_phrase << "\r\n";
    cout << "HTTP/1.1" << " " << status_code << " " << reason_phrase << "\r\n";
    for(auto & iter : respond_header)
    {
        string field =  iter.first;
        string value = iter.second;
        ss << field<<": " << value << "\r\n";
    }
    ss << "\r\n";
    if(status_code == 200 && body != nullptr)
        ss.write(body,body_size);
    ss << "\r\n";
    return  ss.str();
}

HttpBuilder::HttpBuilder(HttpParser parser) {
    body = nullptr;
    httpParser = parser;
}

void HttpBuilder::build() {
    if(httpParser.method == "GET"){
        std::ifstream input(httpParser.request_uri,std::ios::in |std::ios::binary);
        if(input){
            //get length of file
            input.seekg(0, std::ios::end);
            body_size = input.tellg();
            input.seekg(0, std::ios::beg);
            body = new char[body_size];
            //read file
            respond_header["content-length"]= std::to_string(body_size);
            respond_header["Content-Transfer-Encoding"] = "binary";
            input.read(body, body_size);
            status_code = 200;
            reason_phrase = "Ok";

        } else{
            status_code = 404;
            reason_phrase = "Not Found";
            respond_header["content-length"] = std::to_string(0);
        }
        input.close();
    }
    else if(httpParser.method == "POST"){
        unsigned int pos = httpParser.request_uri.find_last_of('\\');
        string directories = httpParser.request_uri.substr(0, pos);
        _mkdir(directories.c_str());
        ofstream os(httpParser.request_uri, ios::binary);
        os.write(httpParser.body->data(),httpParser.body->size());
        status_code = 200;
        reason_phrase = "Ok";
        os.close();
    }
}

HttpBuilder::~HttpBuilder() {
      delete httpParser.body;
      delete body;
}
