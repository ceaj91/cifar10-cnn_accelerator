#define SC_INCLUDE_FX
#include <systemc>
#include <string>
#include <deque>
#include "memory.hpp"

#include "cpu.hpp"
#include "hw.hpp"
#include "types.hpp"

using namespace std;
using namespace sc_core;

int sc_main(int argc, char* argv[])
{
   Mem *memory;
   Cpu *cpu;
   Hardware *hw;

   sc_event do_conv[3];
   sc_event load_param[3];
   sc_event do_maxpool[3];
   deque <hwdata_t> images;
   deque <hwdata_t> weigts;
   deque <hwdata_t> bias;

   
   
   
   
   memory = new Mem("memory", do_conv, load_param, images, weigts, bias);
   cpu = new Cpu("cpu",load_param, do_maxpool, images);
   hw = new Hardware("hw",do_conv,do_maxpool,images, weigts, bias);
   

   sc_start(10, sc_core::SC_NS);
   cout << "Simulation finished at " << sc_time_stamp() << std::endl;
   /*
   for (int i = 0; i < 192; ++i)
   {
      if(i%32 == 0)
         cout<<endl;
      cout<<images[i]<<" ";
   }
   cout<<endl;
   cout<<images.size()<<endl;
   */
   delete memory;
   delete cpu;	
   delete hw;
   return 0;
}
