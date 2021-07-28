cmake_minimum_required(VERSION 3.16)
project(rec_desc)

set(CMAKE_CXX_STANDARD 17)

add_library(runner runner.hpp runner.cpp exceptions/parse_exceptions.cpp exceptions/parse_exceptions.h)
add_library(parser parser.hpp parser.cpp exceptions/parse_exceptions.cpp exceptions/parse_exceptions.h)
add_library(scanner scanner.hpp scanner.cpp exceptions/parse_exceptions.cpp exceptions/parse_exceptions.h)