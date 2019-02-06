#ifndef GPU_H_
#define GPU_H_

#include "../mmu/mmu.hpp"
#include "gpu_mode.hpp"

namespace mattboy::gameboy::gpu {
  
  class GPU
  {
    public:
      GPU();
      ~GPU();

      void Cycle(int cycles, mmu::MMU& mmu);
      void Reset();

    private:
      GPUMode mode_;
      int current_cycles_;
      int line_;
  };

}

#endif