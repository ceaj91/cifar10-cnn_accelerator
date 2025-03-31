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
#include "types.hpp"
#include "../../specification/cpp_implementation/MaxPoolLayer.hpp"
#include "../../specification/cpp_implementation/denselayer.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Cpu)
{

public:
	SC_HAS_PROCESS(Cpu);
	Cpu(sc_module_name name, sc_event *load_param, sc_event* do_maxpool, deque <hwdata_t>& images);
	

protected:
	void software();
	void transform_1D_to_4D(int img_size, int num_of_channels);
	void transform_4D_to_1D(vector4D source_vector,int img_size, int num_of_channels);
	void flatten(vector4D source_vector,int img_size, int num_of_channels);
	
	sc_event *load_param;
	sc_event *do_maxpool;
	
	deque <hwdata_t> &images;
	vector4D images_4D;
	MaxPoolLayer *maxpool[3];
	DenseLayer *dense_layer[2];
	
};


#endif