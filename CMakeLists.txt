cmake_minimum_required(VERSION 2.8)
project (ORIGIN)

include(${ARDUINO_SETTINGS_FILE})
 
set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
 
file(GLOB ORIGIN_SRCS ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.c)
 
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")
 
add_avr_library(${PROJECT_NAME} STATIC ${ORIGIN_SRCS})
###target_link_libraries(${PROJECT_NAME} ZMQ)
