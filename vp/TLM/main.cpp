#define SC_INCLUDE_FX
#include <systemc>
#include <string>
#include <deque>
#include "cpu.hpp"
#include "InterCon.hpp"
#include "memory.hpp"
#include "DMA.hpp"
#include "types.hpp"
#include "hw.hpp"

using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
        sc_fifo<hwdata_t> fifo1(2);
        sc_fifo<hwdata_t> fifo2(2);

        char* image_file_name = argv[1];
        char* labels_file_name = argv[2];

        Cpu cpu("Cpu", image_file_name, labels_file_name);
        Mem memory("memory");
        DMA dma("dma");
        InterCon itc("InterCon");
        Hardware hw("hw");

        cpu.s_cp_i0.bind(itc.s_ic_t);
        itc.s_ic_i0.bind(hw.s_hw_t0);
        itc.s_ic_i1.bind(dma.s_dma_t);
        dma.s_dma_i0.bind(memory.s_mem_t0);
        cpu.s_cp_i1.bind(memory.s_mem_t1);

        dma.p_fifo_out.bind(fifo1);
        hw.p_fifo_in.bind(fifo1);

        dma.p_fifo_in.bind(fifo2);
        hw.p_fifo_out.bind(fifo2);

        hw.p_out(cpu.p_port0);

        #ifdef QUANTUM
        tlm_global_quantum::instance().set(sc_time(10, SC_NS));
        #endif

        cout<<"Starting simulation..."<<endl;

        sc_start(10,SC_SEC);

        cout << "Simulation finished at " << sc_time_stamp() <<endl;

        return 0;
}
