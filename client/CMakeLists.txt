cmake_minimum_required(VERSION 3.19)
project(network_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(network_client main.cpp file_parser.cpp file_parser.h)