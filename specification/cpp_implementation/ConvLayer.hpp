#ifndef ConvLayer_H
#define ConvLayer_H
#include <cstdint>
#include <string>
#include <vector>
#include "common.hpp"
using namespace std;

class ConvLayer
{
private: 
	int filter_size;
	int num_of_channels;
	int num_of_filters;
	std::string padding;
	vector4D conv_filter;
	vector1D bias;
	vector4D output;

	

public:
	ConvLayer(int filter_size,int num_of_channels,int num_of_filters,string padding = "same");

	//~ConvLayer();

	void print_weights();

	void load_weights(const char *filename1,const char *filename2);

	vector4D forward_prop(vector4D input);

	//vector4D pad_img(const vector4D& input_tensor);


};

#endif
