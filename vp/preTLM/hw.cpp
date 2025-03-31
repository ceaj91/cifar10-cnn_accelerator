#ifndef HW_C
#define HW_C
#include "hw.hpp"

Hardware::Hardware(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images,deque <hwdata_t>& weigts,deque <hwdata_t>& bias):sc_module(name), do_conv(do_conv),
																									do_maxpool(do_maxpool),
																									images(images),
																									weigts(weigts),
																									bias(bias)
{
	std::cout << "HW constructed" << std::endl;
	SC_THREAD(proc);
	conv[0]=new ConvLayer("ConvLayer_no_0", &do_conv[0], &do_maxpool[0], images, weigts, bias,3,3,32);
	conv[1]=new ConvLayer("ConvLayer_no_1", &do_conv[1], &do_maxpool[1], images, weigts, bias,3,32,32);
	conv[2]=new ConvLayer("ConvLayer_no_2", &do_conv[2], &do_maxpool[2], images, weigts, bias,3,32,64);
}

void Hardware::proc()
{
	//wait(do_maxpool[0]);
	//cout << "hw " << images.size() << endl;
}




#endif