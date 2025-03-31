#ifndef HW_H
#define HW_H
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
#include "conv.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Hardware)
{

public:
	SC_HAS_PROCESS(Hardware);
	Hardware(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images,deque <hwdata_t>& weigts,deque <hwdata_t>& bias);
	

protected:
	
	sc_event *do_maxpool;
	sc_event *do_conv;

	deque <hwdata_t> &weigts; //potencijalni error
	deque <hwdata_t> &bias;
	deque <hwdata_t> &images;

	void proc();

	ConvLayer *conv[3];
};


#endif