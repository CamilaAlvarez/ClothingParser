file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

pkg_search_module(CVISION REQUIRED cvision)
pkg_search_module(JMSR REQUIRED jmsr)
pkg_search_module(FLANN REQUIRED flann)
pkg_search_module(CAFFE REQUIRED caffe)

find_package(Boost REQUIRED COMPONENTS system)
find_package( OpenCV PATHS /home/calvarez/dependencies/opencv/)
include_directories( ${OpenCV_INCLUDE_DIRS})
include_directories( ${Boost_INCLUDE_DIRS} )

add_library(EXPERIMENT_RUNNER STATIC ${SRC})
target_link_libraries(EXPERIMENT_RUNNER ${Boost_LIBRARIES} ${CAFFE_LIBRARIES} ${CVISION_LIBRARIES} ${JMSR_LIBRARIES} ${FLANN_LIBRARIES} ${OpenCV_LIBS})
target_compile_options(EXPERIMENT_RUNNER PUBLIC ${CAFFE_CFLAGS_OTHER} ${CVISION_CFLAGS_OTHER} ${JMSR_CFLAGS_OTHER} ${FLANN_CFLAGS_OTHER} )

target_include_directories(EXPERIMENT_RUNNER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CVISION_INCLUDE_DIRS} ${JMSR_INCLUDE_DIRS} ${FLANN_INCLUDE_DIRS} ${CAFFE_INCLUDE_DIRS})
set_target_properties(EXPERIMENT_RUNNER PROPERTIES 
			LINKER_LANGUAGE CXX)
