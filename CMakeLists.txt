#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#

set(CMAKE_MODULE_PATH    ${CMAKE_SOURCE_DIR}/../../arduino-cmake/cmake/modules)  # CMake module search path
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../../arduino-cmake/cmake/toolchains/Arduino.cmake) # Arduino Toolchain
set(ARDUINO_SDK_VERSION 1.0)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoExample C CXX)
find_package(Arduino 1.0 REQUIRED)

#====================================================================#
#  Settings                                                          #
#====================================================================#
set(FIRMWARE_NAME arduinorp6)

set(${FIRMWARE_NAME}_BOARD uno)               # Arduino Target board
# set(${FIRMWARE_NAME}_SKETCHES arduinorp6.ino)
set(${FIRMWARE_NAME}_SRCS widget.cpp menu.cpp gui.cpp interface.cpp mainstatwidget.cpp arduinorp6.cpp)  # Firmware sources
set(${FIRMWARE_NAME}_PORT /dev/ttyACM0)            # Serial upload port
# set(${FIRMWARE_NAME}_LIBS ColorLCDShield)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libraries)

#====================================================================#
#  Target generation                                                 #
#====================================================================#
generate_arduino_firmware(${FIRMWARE_NAME})
