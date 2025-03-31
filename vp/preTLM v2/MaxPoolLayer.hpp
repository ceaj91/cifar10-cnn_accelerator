#ifndef MAXPOOLLAYER_H
#define MAXPOOLLAYER_H
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
using namespace std;
using namespace sc_core;

SC_MODULE(MaxPoolLayer)
{

public:
	SC_HAS_PROCESS(MaxPoolLayer);
	MaxPoolLayer(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images, int img_size, int num_of_channels);
	
    // Pravim drugi konstruktor koji umjesto do_conv kao parametar dobija do_dense
    // Ovaj konstruktor sluzi za MaxPoolLayer2 (poslednji layer) koji javlja CPU da je HW gotov sa radom
    //MaxPoolLayer(sc_module_name name, sc_event *do_dense, sc_event* do_maxpool,deque <hwdata_t>& images, int img_size, int num_of_channels);

protected:
	
	sc_event *do_maxpool;
	sc_event *do_conv;

	deque <hwdata_t> &images;

    int img_size;
    int num_of_channels;

    // Hardcodujemo jer znamo da je pool size uvijek 2
	int pool_size = 2;
	
	void forward_prop();

};


#endif