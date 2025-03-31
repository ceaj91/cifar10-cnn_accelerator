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
#include "addresses.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
#include "../../specification/cpp_implementation/MaxPoolLayer.hpp"
#include "../../specification/cpp_implementation/denselayer.hpp"
using namespace std;
using namespace sc_core;

SC_MODULE(Cpu)
{

	public:
		SC_HAS_PROCESS(Cpu);
		Cpu(sc_module_name name, char* image_file_name, char* labels_file_name);
		tlm_utils::simple_initiator_socket<Cpu> s_cp_i0;
		tlm_utils::simple_initiator_socket<Cpu> s_cp_i1;
		sc_out<sc_logic> p_port0;
		sc_signal<sc_logic> sig0;

		

	protected:
		void transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels);
		void transform_4D_to_1D(vector4D source_vector,vector1D& dest_vector,int img_size, int num_of_channels);
		void flatten(vector4D source_vector,vector2D &dest_vector,int img_size, int num_of_channels);

		void extract_parameters();
		void extract_pictures(int picture_number);
		void extract_labels();
		void format_image(int img_size, int num_of_channels);
		int num_of_lines(const char *);

		void software();
		int ip_command;
		sc_logic tmp_sig;
		MaxPoolLayer *maxpool[3];
		DenseLayer *dense_layer[2];

		vector<hwdata_t> ram;

		vector<float> labels;
		char* image_file_name;
		char* labels_file_name;

		void pad_img(int img_size, int num_of_channels);	
};


#endif