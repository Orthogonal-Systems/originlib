cmake_minimum_required(VERSION 2.8)
project (origin_examples)

include(${ARDUINO_SETTINGS_FILE})
 
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
 
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

### add example executable directories
add_avr_executable(origin-test origin-test/origin-test.cpp)

### add the dependant libraries
### zmq monitoring server
avr_target_link_libraries(origin-test ARDUINO)
avr_target_link_libraries(origin-test UIPETHERNET)
avr_target_link_libraries(origin-test ZMQ)
avr_target_link_libraries(origin-test TIME)
avr_target_link_libraries(origin-test ORIGIN)
