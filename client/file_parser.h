//
// Created by zezo on 11/10/2021.
//

#ifndef NETWORK_CLIENT_FILE_PARSER_H
#define NETWORK_CLIENT_FILE_PARSER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
pair<int,char*> get_body(string file_path);
string create_http_command(string method,string file_path);
vector<vector<string>> parse(string file_path);
#endif //NETWORK_CLIENT_FILE_PARSER_H
