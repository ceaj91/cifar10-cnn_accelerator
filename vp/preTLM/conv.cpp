#ifndef CONV_C
#define CONV_C
#include "conv.hpp"

ConvLayer::ConvLayer(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images,
					 deque <hwdata_t>& weigts,deque <hwdata_t>& bias, int filter_size, int num_of_channels, int num_of_filters): sc_module(name), do_conv(do_conv),
																									do_maxpool(do_maxpool),
																									images(images),
																									weigts(weigts),
																									bias(bias),
																									filter_size(filter_size),
																									num_of_channels(num_of_channels),
																									num_of_filters(num_of_filters)
{
	cout<<"Conv costructed"<<endl;
	SC_THREAD(forward_prop);
}																										


void ConvLayer::forward_prop()
{
	wait(*do_conv);
	deque<hwdata_t> output;
	pad_img(images);
	int img_size;
	img_size = images.size();


	switch(img_size){
	case 34*34*3:
		img_size = 34;
		break;
	case 18*18*32:
		img_size = 18;
		break;
	case 10*10*32:
		img_size = 10;
		break;
	default:
		cout<<"ERROR in image_size";
		//return -1;
		break;
	}
	//izdvoj 3x3 matricu iz images

	for (int i = 0; i < img_size-2; i++)
	{
		for (int j = 0; j < img_size-2; j++)
		{

			deque<hwdata_t> image_slice;
			image_slice.clear();
			for (int channel_slice = 0; channel_slice < this->num_of_channels; ++channel_slice)
			{	
				for (int i_slice = i; i_slice < i+this->filter_size; ++i_slice)
				{
					for (int j_slice = j; j_slice < j+this->filter_size; ++j_slice)
					{

						image_slice.push_back(images[channel_slice * (img_size * img_size) + i_slice*img_size + j_slice]);			
					}
				}
			}

			for (int filter = 0; filter < this->num_of_filters; ++filter)
			{
				hwdata_t conv_sum = 0;
				for (int channel = 0; channel < this->num_of_channels; ++channel)
					{
						for (int i_slice = 0; i_slice < this->filter_size; ++i_slice)
						{
							for (int j_slice = 0; j_slice < this->filter_size; ++j_slice)
							{
            					
								conv_sum = conv_sum + image_slice[channel * (this->filter_size * this->filter_size) + i_slice*this->filter_size + j_slice] * weigts[filter*(this->filter_size*this->filter_size*this->num_of_channels)  + channel * (this->filter_size * this->filter_size) + i_slice*this->filter_size + j_slice];
							}
						}
					}
				if(conv_sum + bias[filter] > 0)
					output.push_back(conv_sum + bias[filter]);
				else	
					output.push_back(0);
			}
		}
	}
	
	
	
	images.clear();
	images = output;
	
	(*do_maxpool).notify(SC_ZERO_TIME);
	
}

void ConvLayer::pad_img(deque <hwdata_t> &images)
{
	int img_size;
	img_size = images.size();

	switch(img_size){
	case 32*32*3:
		img_size = 32;
		break;
	case 16*16*32:
		img_size = 16;
		break;
	case 8*8*32:
		img_size = 8;
		break;
	default:
		cout<<"ERROR in image_size";
		//return -1;
		break;
	}

	for(int channel = 0 ; channel < this->num_of_channels;channel++)
    {
        for (int i = 0; i < img_size+2; i++)
        {
            images.emplace(images.begin() + ((channel)*(img_size+2)*(img_size+2) + i), 0);
        }

        for(int rows = 1 ; rows < img_size + 1;rows++)
        {
            int pos1 = (channel)*(img_size+2)*(img_size+2) + rows*img_size + rows*2;
            int pos2 = (channel)*(img_size+2)*(img_size+2) + rows*img_size + rows*2+1+img_size;
            images.emplace(images.begin() + pos1, 0);
            images.emplace(images.begin() + pos2, 0);
        }

        for (int i = 0; i < img_size+2; i++)
        {
            images.emplace(images.begin() + ((channel)*(img_size+2)*(img_size+2) + (img_size+2)*(img_size+1) + i), 0);
        }
    }
    
    

}





#endif