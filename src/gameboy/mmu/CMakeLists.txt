SET(mmu_src_files
  mmu.hpp
  mmu.cpp
  cartridge.hpp
  cartridge.cpp
)

ADD_LIBRARY(mmu ${mmu_src_files})