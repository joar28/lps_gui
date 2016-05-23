cmake_minimum_required(VERSION 3.3)
project(uart_gui)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
        game.cpp                game.h
        settings.cpp            settings.h

        resourceLoader.cpp      resourceLoader.h

        states/baseState.cpp    states/baseState.h
        states/stateMenu.cpp    states/stateMenu.h
        states/state2D.cpp      states/state2D.h
        Device.cpp              Device.h
        udp_server.cpp udp_server.h)

include_directories("/usr/include/eigen3")
#find_package(Eigen3 REQUIRED)
#include_directories(EIGEN3_INCLUDE_DIR)

add_executable(uart_gui ${SOURCE_FILES})

target_link_libraries (
        uart_gui
        sfml-system
        sfml-window
        sfml-graphics
        sfml-audio
        #jsoncpp
)