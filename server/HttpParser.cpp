//
// Created by zezo on 11/7/2021.
//

#include "HttpParser.h"
vector<char> HttpParser::parse(vector<char> input) {
    unsigned int last_bf_body=0;
    string stream(input.data());
    std::string delimiter = "\r\n"; //CLRF Delimiter
    size_t pos = stream.find(delimiter);
        last_bf_body += pos+2;
        std::string request_line = stream.substr(0, pos);
        size_t req_line_pos = request_line.find(" ");
        cout << request_line << endl;
        method = request_line.substr(0, req_line_pos);
        request_line.erase(0, req_line_pos + 1);
        req_line_pos = request_line.find(" ");
        request_uri = request_line.substr(1, req_line_pos);
        request_line.erase(0, req_line_pos + 1);
        stream.erase(0, pos + delimiter.length());


    pos = stream.find(delimiter);
    while ( stream.substr(0,pos) != "" && pos != std::string::npos) {
        string header_line = stream.substr(0, pos);
        cout << header_line << endl;
        last_bf_body += pos+2;
        int header_pos= header_line.find(":");
        string field = header_line.substr(0,header_pos);
        string  value = header_line.substr(header_pos+2);
        header_map[field] = value;
        stream.erase(0, pos + delimiter.length());
        pos = stream.find(delimiter);
    }
    last_bf_body +=2;
    if( method == "POST" && pos != string::npos) {
        stream.erase(0, delimiter.length() + pos);
        vector<char>::const_iterator first = input.begin() + last_bf_body;

        vector<char>::const_iterator last = input.begin()  + last_bf_body + std::stoi(header_map["Content-Length"]);
        body = new vector<char>(first, last);
        first = last + 2 ;
        last= input.end();
        input = vector<char>(first,last);

    }else if(pos != string::npos){
        vector<char>::const_iterator first = input.begin() + last_bf_body;

        vector<char>::const_iterator last = input.end();
        input = vector<char>(first,last);
    }
    return input;

}

HttpParser::HttpParser() {

}
HttpParser::~HttpParser(){
}
