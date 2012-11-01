cmake_minimum_required(VERSION 2.8)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../../arduino-cmake/cmake/ArduinoToolchain.cmake)

project(arduinorp6 C CXX)

# This configuration file outlines some of the ways Arduino CMake
# can be used. For a detailed explenation of all the options please
# reade README.rst.

set(ARDUINO_DEFAULT_BOARD uno) # Default Board ID, when not specified
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0) # Default Port, when not specified

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libraries)

generate_arduino_firmware(arduinorp6
    SRCS widget.cpp menu.cpp gui.cpp interface.cpp logwidget.cpp mainstatwidget.cpp arduinorp6.cpp)

