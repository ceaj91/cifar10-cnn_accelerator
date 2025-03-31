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
#include "addresses.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
using namespace std;
using namespace sc_core;

SC_MODULE(Hardware)
{

public:
	SC_HAS_PROCESS(Hardware);
	Hardware(sc_module_name name);
	tlm_utils::simple_target_socket<Hardware> s_hw_t0;
	
	sc_port<sc_fifo_in_if<hwdata_t>> p_fifo_in;
	sc_port<sc_fifo_out_if<hwdata_t>> p_fifo_out;
	sc_out<sc_logic> p_out;

protected:
    void b_transport0(pl_t&, sc_time&);
	void proc();

	// Conv layers are instanced as 3 seperated processes
	void conv0();
	void conv1();
	void conv2();

	vector <hwdata_t> weigts; 
	vector <hwdata_t> bias;
	vector <hwdata_t> input_image;
	vector <hwdata_t> output_image;
	int command_reg;
	sc_logic toggle;
	sc_logic sig_conv0;
	sc_logic sig_conv1;
	sc_logic sig_conv2;	

	int conv1_counter;
	int conv2_counter;
};


#endif