cmake_minimum_required(VERSION 3.5)
project(Street2Shop)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


#Setea variables -> en este caso la variable SOURCE_FILES
set(SOURCE_FILES main.cpp)
add_executable(Street2ShopParser ${SOURCE_FILES})

