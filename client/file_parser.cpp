//
// Created by zezo on 11/10/2021.
//
#include <stdlib.h>
#include <stdio.h>

#include "file_parser.h"
int size;
pair<int,char*> get_body(string file_path){
    std::ifstream input(file_path.substr(1),std::ios::in |std::ios::binary);
    pair<int,char*> p;
    p.first =0 ;
    if(input){
        char* body;
        input.seekg(0, std::ios::end);
        int body_size = input.tellg();
        body = new char[body_size];
        input.seekg(0, std::ios::beg);
        input.read(body, body_size);
        p.first = body_size;
        p.second = body;
        return p;
    }else{
        p.second = "NOT FOUND";
        return p;
    }
}
 string create_http_command(string method,string file_path){
    if(method == "client_get"){
        stringstream command;
        command << "GET " << file_path << " HTTP/1.1\r\n";
        command << "\r\n";
        return command.str();
    }else if(method == "client_post") {
        stringstream command;
        command << "POST " << file_path << " HTTP/1.1\r\n";

        auto b = get_body( file_path);
        command << "Content-Length: " << b.first;
        command << "\r\n\r\n";
        if (b.first != 0) {
        command.write(b.second, b.first);
        cout << "BODY size = " << b.first <<endl;
        delete b.second;
        command << "\r\n";
        return command.str();
        } else{
                return "NOT FOUND";
        }

    }else{
        return "INVALID";
    }
}
vector<vector<string>> parse(string file_path){
    ifstream input(file_path,std::ios::in |std::ios::binary);
    vector<vector<string>> commands;
    if(input){
        for( std::string line; getline( input, line ); )
        {
            vector<string> comand;
            int pos = line.find(" ");
            string method = line.substr(0,pos);
            comand.push_back(method);
            line.erase(0,pos+1);
            pos = line.find(" ");
            string req_file_path = line.substr(0,pos);
            comand.push_back(req_file_path);
            line.erase(0,pos+1);
            pos = line.find(" ");
            string host_name = line.substr(0,pos);
            if(host_name[host_name.size()-1] == '\r'){
                host_name = host_name.substr(0,host_name.size()-1);
                comand.push_back(host_name);
                comand.push_back("80");
            }else {
                comand.push_back(host_name);
                int bf = line.size();
                line.erase(0, pos + 1);
                if(bf == line.size()) {
                    string port = "80";
                    comand.push_back(port);
                }else {
                    pos = line.find('\r');
                    line = line.substr(0, pos);
                    comand.push_back(line);
                }

            }
            commands.push_back(comand);
        }
    }else{
        cout << "Error file not found";
        exit(-1);
    }
    return commands;
}
