file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

pkg_search_module(CVISION REQUIRED cvision)
pkg_search_module(JMSR REQUIRED jmsr)
pkg_search_module(FLANN REQUIRED flann)
pkg_search_module(CAFFE REQUIRED caffe)

find_package(Boost REQUIRED COMPONENTS system)
find_package( OpenCV PATHS /home/calvarez/dependencies/opencv/)
find_package (Threads)
FIND_PACKAGE( OpenMP REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories( ${Boost_INCLUDE_DIRS} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
add_library(NETWORK_MANAGER STATIC ${SRC})
target_link_libraries(NETWORK_MANAGER ${Boost_LIBRARIES} ${CAFFE_LIBRARIES} ${CVISION_LIBRARIES} ${JMSR_LIBRARIES} ${FLANN_LIBRARIES} ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} )
target_compile_options(NETWORK_MANAGER PUBLIC ${CAFFE_CFLAGS_OTHER} ${CVISION_CFLAGS_OTHER} ${JMSR_CFLAGS_OTHER} ${FLANN_CFLAGS_OTHER} ${OpenMP_C_FLAGS})
target_include_directories(NETWORK_MANAGER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CVISION_INCLUDE_DIRS} ${JMSR_INCLUDE_DIRS} ${FLANN_INCLUDE_DIRS} ${CAFFE_INCLUDE_DIRS})
set_target_properties(NETWORK_MANAGER PROPERTIES
        LINKER_LANGUAGE CXX)