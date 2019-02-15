#ifndef GPU_H_
#define GPU_H_

#include "MMU.h"
#include "GPUMode.h"

namespace mattboy {

	class GPU
	{
	public:
		GPU();
		~GPU();

		void Cycle(int cycles, MMU& mmu, InterruptHandler& interrupt_handler);
		void Reset();

	private:
		GPUMode mode_;
		int current_cycles_;
		int line_;
	};

}

#endif