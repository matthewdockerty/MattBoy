CMAKE_MINIMUM_REQUIRED(VERSION 3.13.3)
PROJECT(MattBoy_exe)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

ADD_SUBDIRECTORY(gameboy)

SET(mattboy_src_files
    mattboy.cpp
    util.hpp
    util.cpp
)

ADD_EXECUTABLE(MattBoy ${mattboy_src_files})

TARGET_LINK_LIBRARIES(MattBoy gameboy)
