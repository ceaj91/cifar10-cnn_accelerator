#ifndef MEM_H
#define MEM_H
#define SC_INCLUDE_FX
#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "types.hpp"
#include "addresses.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"

using namespace std;
using namespace sc_core;

SC_MODULE(Mem)
{

	public:
		SC_HAS_PROCESS(Mem);
		Mem(sc_module_name name);

		tlm_utils::simple_target_socket<Mem> s_mem_t0; 
		tlm_utils::simple_target_socket<Mem> s_mem_t1;
		void b_transport(pl_t&, sc_time&);

	protected:
		vector<hwdata_t> ram;
		void mem_init();
};


#endif
