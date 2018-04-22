cmake_minimum_required(VERSION 2.8)
project (origin_examples)

include(${ARDUINO_SETTINGS_FILE})
 
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
 
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

### add example executable directories
add_avr_executable(origin_test origin_test/origin_test.cpp)
add_avr_executable(thermistor thermistor/thermistor.cpp)

### add the dependant libraries
### zmq monitoring server
avr_target_link_libraries(origin_test ARDUINO)
avr_target_link_libraries(origin_test UIPETHERNET)
#avr_target_link_libraries(origin_test ZMQ)
avr_target_link_libraries(origin_test TIME)
avr_target_link_libraries(origin_test ORIGIN)

avr_target_link_libraries(thermistor ARDUINO)
avr_target_link_libraries(thermistor UIPETHERNET)
avr_target_link_libraries(thermistor TIME)
avr_target_link_libraries(thermistor ORIGIN)
