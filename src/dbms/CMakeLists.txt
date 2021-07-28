cmake_minimum_required(VERSION 3.16)
project(dbms)

set(CMAKE_CXX_STANDARD 17)

add_library(dbms table.hpp table.cpp exceptions/dbms_exceptions.cpp exceptions/dbms_exceptions.h)