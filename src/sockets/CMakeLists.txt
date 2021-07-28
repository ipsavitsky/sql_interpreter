cmake_minimum_required(VERSION 3.16)
project(sockets)

set(CMAKE_CXX_STANDARD 17)

add_library(sockets sockets.hpp sockets.cpp exceptions/sock_exceptions.cpp exceptions/sock_exceptions.h)