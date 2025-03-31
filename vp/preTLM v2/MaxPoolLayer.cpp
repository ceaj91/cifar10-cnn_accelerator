#ifndef MAXPOOLLAYER_C
#define MAXPOOLLAYER_C
#include "MaxPoolLayer.hpp"

MaxPoolLayer::MaxPoolLayer(sc_module_name name, sc_event *do_conv, sc_event* do_maxpool, deque <hwdata_t>& images, int img_size, int num_of_channels): sc_module(name), do_conv(do_conv),
																									do_maxpool(do_maxpool),
																									images(images),
                                                                                                    img_size(img_size),
                                                                                                    num_of_channels(num_of_channels)																					
{
	cout<<"MaxPool with do_conv costructed"<<endl;
	SC_THREAD(forward_prop);
}		
/*
MaxPoolLayer::MaxPoolLayer(sc_module_name name, sc_event *do_dense, sc_event* do_maxpool, deque <hwdata_t>& images, int img_size, int num_of_channels): sc_module(name), do_dense(do_dense),
																									do_maxpool(do_maxpool),
																									images(images),
                                                                                                    img_size(img_size),
                                                                                                    num_of_channels(num_of_channels)																					
{
	cout<<"MaxPool with do_dense costructed"<<endl;
	SC_THREAD(forward_prop);
}	
*/

void MaxPoolLayer::forward_prop()
{
    wait(*do_maxpool);

	deque<hwdata_t> output;
    deque<hwdata_t> image_slice;

	output.clear();
    image_slice.clear();
    hwdata_t temp_max;


    for(int channel = 0; channel < num_of_channels; channel++)
    {
    for(int row = 0; row < img_size; row += 2)
    {
        for(int col = 0; col < img_size; col += 2)
        {
            image_slice.clear();

            // U image_slice uzimam 2x2 slice u kojem trazim max
            //for(int channel = 0; channel < num_of_channels; channel++)
            //{
                for(int r = row; r < row + pool_size; r++)
                {
                    for(int c = col; c < col + pool_size; c++)
                    {
                        image_slice.push_back(images[channel + r*num_of_channels*img_size + c*num_of_channels]);
                    }
                }
            //}

            // MaxPooling 2x2 image slice-a
            temp_max = image_slice[0];
            for(int i = 1; i < image_slice.size(); i++)
            {
                if(image_slice[i] > temp_max) temp_max = image_slice[i];
            }

            output.push_back(temp_max);
        }
    }
    }
    

    images.clear();
    images = output;

    (*do_conv).notify(SC_ZERO_TIME);

}

#endif