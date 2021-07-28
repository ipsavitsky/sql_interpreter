cmake_minimum_required(VERSION 3.16)
project(logger)

set(CMAKE_CXX_STANDARD 17)

add_library(logger logger.hpp logger.cpp)