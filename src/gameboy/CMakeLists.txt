SET(gameboy_src_files
    gameboy.hpp
    gameboy.cpp
)

ADD_SUBDIRECTORY(mmu)
ADD_SUBDIRECTORY(cpu)

ADD_LIBRARY(gameboy ${gameboy_src_files})
TARGET_LINK_LIBRARIES(gameboy mmu cpu)
