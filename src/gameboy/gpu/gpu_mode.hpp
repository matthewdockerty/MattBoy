#ifndef GPU_MODE_H_
#define GPU_MODE_H_

namespace mattboy::gameboy::gpu {

  enum GPUMode
  {
    HBLANK,
    VBLANK,
    OAM,
    VRAM
  };

}

#endif