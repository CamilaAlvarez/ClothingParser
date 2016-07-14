cmake_minimum_required(VERSION 3.5)
project(WhereToBuyItEvaluation)

find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_subdirectory(src)

set(SOURCE_FILES main.cpp)
add_executable(WhereToBuyItEvaluation ${SOURCE_FILES})

target_link_libraries(WhereToBuyItEvaluation EXPERIMENT_RUNNER)
target_include_directories(WhereToBuyItEvaluation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

