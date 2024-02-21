cmake_minimum_required(VERSION 3.24)
project(BMS_DataBase)

set(CMAKE_CXX_STANDARD 20)

add_executable(BMS_DataBase main.cpp DataBase.cpp DataBase.h Books.h Publishers.h GarbageCollector.h)