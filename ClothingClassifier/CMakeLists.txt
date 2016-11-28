cmake_minimum_required(VERSION 3.5)
project(ClothingClassifier)
find_package(PkgConfig REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_subdirectory(src)

set(SOURCE_FILES main.cpp)
add_executable(classifier-evaluation ${SOURCE_FILES})

target_link_libraries(classifier-evaluation NETWORK_MANAGER)
target_include_directories(classifier-evaluation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
