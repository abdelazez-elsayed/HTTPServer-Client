//
// Created by zezo on 11/7/2021.
//

#ifndef NETWORK_ASSIGNMENT1_HTTPPARSER_H
#define NETWORK_ASSIGNMENT1_HTTPPARSER_H
#include <cstring>
#include <iostream>
#include <map>
#include <utility>
#include <cassert>
#include <vector>
#include <string>
using namespace std;
class HttpParser {
public:
    HttpParser();
    ~HttpParser();
    vector<char> parse(vector<char> input);
    map<string,string> header_map ;
    string method;
    string request_uri;
    vector<char>* body = nullptr;
private:


};


#endif //NETWORK_ASSIGNMENT1_HTTPPARSER_H
