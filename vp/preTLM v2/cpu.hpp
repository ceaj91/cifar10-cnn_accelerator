#ifndef CPU_H
#define CPU_H
#define SC_INCLUDE_FX

#include <iostream>
#include <systemc>
#include <string>
#include <fstream>
#include <deque>
#include <vector>
#include <array>
#include <algorithm>
#include "../preTLM/types.hpp"

#include "../../specification/cpp_implementation/denselayer.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Cpu)
{

public:
	SC_HAS_PROCESS(Cpu);
	Cpu(sc_module_name name, sc_event *load_image, sc_event* do_dense, deque <hwdata_t>& images);
	

protected:
	void software();
	void flatten(int img_size, int num_of_channels);
	
	sc_event *load_image;
	sc_event *do_dense;
	
	deque <hwdata_t> &images;
	DenseLayer *dense_layer[2];
	
};


#endif
