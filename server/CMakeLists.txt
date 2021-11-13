cmake_minimum_required(VERSION 3.19)
project(Network_assignment1)

set(CMAKE_CXX_STANDARD 14)

add_executable(Network_assignment1 server.cpp HttpParser.cpp HttpParser.h HttpBuilder.cpp HttpBuilder.h SocketHandler.cpp SocketHandler.h)