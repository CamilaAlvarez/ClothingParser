cmake_minimum_required(VERSION 3.5)
project(SiameseDataCropper)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(OpenCV REQUIRED)
if (!OPENCV_FOUND)
    find_package( OpenCV PATHS /home/calvarez/dependencies/opencv/)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SOURCE_FILES *.cpp)
add_executable(SiameseDataCropper ${SOURCE_FILES})
target_link_libraries( SiameseDataCropper ${OpenCV_LIBS} )