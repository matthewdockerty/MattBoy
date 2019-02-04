SET(mmu_src_files
  mmu.hpp
  mmu.cpp
  cartridge.hpp
  cartridge.cpp
  cartridge_type.hpp
  mbc.hpp
  mbc_none.hpp
  mbc_none.cpp
)

ADD_LIBRARY(mmu ${mmu_src_files})