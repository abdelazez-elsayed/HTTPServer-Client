//
// Created by zezo on 11/7/2021.
//

#ifndef NETWORK_ASSIGNMENT1_HTTPBUILDER_H
#define NETWORK_ASSIGNMENT1_HTTPBUILDER_H
#include <cstring>
#include <iostream>
#include <map>
#include <utility>
#include "HttpParser.h"
class HttpBuilder {
public:
    HttpBuilder();
    ~HttpBuilder();
    explicit HttpBuilder(HttpParser parser);
    void build();
    string to_string();

private:
    int status_code;
    string reason_phrase;
    HttpParser httpParser;
    map<string,string> respond_header;
    char* body;
    size_t body_size;
};


#endif //NETWORK_ASSIGNMENT1_HTTPBUILDER_H
