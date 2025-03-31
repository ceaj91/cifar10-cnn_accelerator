#ifndef CONV_H
#define CONV_H
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
using namespace std;
using namespace sc_core;

SC_MODULE(ConvLayer)
{

public:
	SC_HAS_PROCESS(ConvLayer);
	ConvLayer(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images,deque <hwdata_t>& weigts,deque <hwdata_t>& bias, int filter_size, int num_of_channels, int num_of_filters);
	

protected:
	
	sc_event *do_maxpool;
	sc_event *do_conv;

	deque <hwdata_t> &weigts; //potencijalni error
	deque <hwdata_t> &bias;
	deque <hwdata_t> &images;

	int filter_size;
	int num_of_channels;
	int num_of_filters;
	
	void pad_img(deque <hwdata_t> &images);
	void forward_prop();

};


#endif