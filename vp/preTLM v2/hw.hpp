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
#include "../preTLM/types.hpp"
#include "conv.hpp"
#include "MaxPoolLayer.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Hardware)
{

public:
	SC_HAS_PROCESS(Hardware);
	Hardware(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, sc_event* do_dense, deque <hwdata_t>& images,deque <hwdata_t>& weigts0, deque <hwdata_t>& weigts1, deque <hwdata_t>& weigts2,
			deque <hwdata_t>& bias0, deque <hwdata_t>& bias1, deque <hwdata_t>& bias2);
	

protected:
	
	sc_event *do_maxpool;
	sc_event *do_conv;
	sc_event *do_dense;

	deque <hwdata_t> &weigts0; 
	deque <hwdata_t> &weigts1; 
	deque <hwdata_t> &weigts2; 
	deque <hwdata_t> &bias0;
	deque <hwdata_t> &bias1;
	deque <hwdata_t> &bias2;
	deque <hwdata_t> &images;

	void proc();

	ConvLayer *conv[3];
	MaxPoolLayer *maxpool[3];
};


#endif