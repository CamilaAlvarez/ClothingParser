cmake_minimum_required(VERSION 3.5)
project(Street2Shop)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	find_package( OpenCV REQUIRED )
ELSE()
	find_package( OpenCV PATHS /home/calvarez/dependencies/opencv/)
	include_directories( ${OpenCV_INCLUDE_DIRS} )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++11 -std=c++0x -pthread")

#Setea variables -> en este caso la variable SOURCE_FILES
set(SOURCE_FILES main.cpp)
add_executable(Street2ShopParser ${SOURCE_FILES})
target_link_libraries(Street2ShopParser ${OpenCV_LIBS})
set_target_properties(Street2ShopParser PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
