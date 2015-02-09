set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arduino-cmake/cmake/ArduinoToolchain.cmake)

cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME controller_firmware)

project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall")

set(SERIAL_BAUD 9600)

add_definitions(-DSERIAL_BAUD=${SERIAL_BAUD})

# Arduino settings
set(${PROJECT_NAME}_SRCS src/controller_firmware.cpp)
set(${PROJECT_NAME}_PORT /dev/ttyACM0)
set(${PROJECT_NAME}_BOARD micro)
set(${PROJECT_NAME}_SERIAL screen ${${PROJECT_NAME}_PORT} ${SERIAL_BAUD})

generate_arduino_firmware(${PROJECT_NAME})
