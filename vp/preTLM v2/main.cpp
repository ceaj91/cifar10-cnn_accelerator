#define SC_INCLUDE_FX
#include <systemc>
#include <string>
#include <deque>
#include "memory.hpp"
#include "cpu.hpp"
#include "hw.hpp"
#include "../preTLM/types.hpp"

using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
   Mem *memory;
   Cpu *cpu;
   Hardware *hw;

   sc_event do_conv[3];
   sc_event load_image[1];
   sc_event do_maxpool[3];
   sc_event do_dense[1];
   
   deque <hwdata_t> images;
   deque <hwdata_t> weigts0;
   deque <hwdata_t> weigts1;
   deque <hwdata_t> weigts2;
   deque <hwdata_t> bias0;
   deque <hwdata_t> bias1;
   deque <hwdata_t> bias2;

   memory = new Mem("memory", do_conv, load_image, images, weigts0, weigts1, weigts2, bias0, bias1, bias2);
   cpu = new Cpu("cpu",load_image, do_dense, images);
   hw = new Hardware("hw",do_conv,do_maxpool, do_dense, images, weigts0, weigts1, weigts2, bias0, bias1, bias2);
   

   sc_start(10, sc_core::SC_NS);
   cout << "Simulation finished at " << sc_time_stamp() << std::endl;
   
   delete memory;
   delete cpu;	
   delete hw;
   return 0;
}
