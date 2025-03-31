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
#include "../preTLM/types.hpp"
#include "../preTLM/addresses.hpp"

using namespace std;
using namespace sc_core;

SC_MODULE(Mem)
{

public:
	SC_HAS_PROCESS(Mem);
	Mem(sc_module_name name, sc_event *do_conv, sc_event *load_image, deque <hwdata_t>& images, deque <hwdata_t>& weigts0, deque <hwdata_t>& weigts1, deque <hwdata_t>& weigts2,
			deque <hwdata_t>& bias0, deque <hwdata_t>& bias1, deque <hwdata_t>& bias2);
	int num_of_lines(const char *);
protected:
	deque <hwdata_t> &weigts0; 
	deque <hwdata_t> &weigts1; 
	deque <hwdata_t> &weigts2; 
	deque <hwdata_t> &bias0;
	deque <hwdata_t> &bias1;
	deque <hwdata_t> &bias2;
	deque <hwdata_t> &images;
	deque <hwdata_t> ram;

	sc_event *do_conv;
	sc_event *load_image;

	void file_extract();
	void grab_from_mem();
};


#endif
