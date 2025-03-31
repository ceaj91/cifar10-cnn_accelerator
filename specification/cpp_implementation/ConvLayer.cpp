#include "ConvLayer.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include "common.hpp"

ConvLayer::ConvLayer(int filter_size,int num_of_channels,int num_of_filters,string padding): conv_filter(filter_size, std::vector<std::vector<std::vector<float>>>(filter_size, std::vector<std::vector<float>>(num_of_channels, std::vector<float>(num_of_filters, 0.0))))
    , bias(num_of_filters, 0.0)
{

	this->filter_size = filter_size;
	this->num_of_channels = num_of_channels;
	this->num_of_filters=num_of_filters;
	this->padding = padding;	
	this->conv_filter=vector4D(filter_size, vector3D(filter_size,vector2D(num_of_channels,vector1D(num_of_filters,0.0))));
    this->bias = vector1D(num_of_filters,0.0);
	//this->output = vector4D(batch_size,vector3D(input_height,vector2D(input_width,vector1D(num_of_filters,0.0))));

}






void ConvLayer::print_weights(void){
for (int i = 0; i < this->num_of_filters; i++)
	{
		std::cout<<"Filter: "<<i<<std::endl;
		for (int j = 0; j < this->num_of_channels; j++)
		{
			std::cout<<"\tChannel: "<<j<<std::endl;
			for (int k = 0; k < this->filter_size; k++)
			{
				for(int l=0;l<this->filter_size;l++){
				std::cout<<setprecision(18)<<conv_filter[k][l][j][i]<<" ";
				}
				std::cout<<std::endl;
			}
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}
}

/*
vector4D ConvLayer::pad_img(const vector4D& input_tensor){

	
	int height = input_tensor[0].size();
	int width = input_tensor[0][0].size();
	int padded_height = height + 2; 
	int padded_width = width + 2;
	int pad_width = 1;
	vector4D padded_array(input_tensor.size(),vector3D(padded_height,vector2D(padded_width,vector1D(input_tensor[0][0][0].size(),0.0))));
	for (int i = 1; i < height + pad_width; i++)
		{
			for (int j = 1; j < width + pad_width; j++)
			{
				for(int k =0; k < input_tensor.size();k++)
				{
					for (int n = 0; n < input_tensor[0][0][0].size(); n++)
					{
						padded_array[k][i][j][n] = input_tensor[k][i-pad_width][j-pad_width][n];					
					}

				}
			}
		}
	return padded_array;	
}*/


vector4D ConvLayer::forward_prop(vector4D input_tensor)
{
	int batch_size = input_tensor.size();
	int input_height = input_tensor[0].size()- 2;
	int input_width = input_tensor[0][0].size()-2;
	int input_channels = input_tensor[0][0][0].size();
	int stride =1;
	//uvek ce "same" bit
	this->output = vector4D(batch_size,vector3D(input_height,vector2D(input_width,vector1D(this->num_of_filters,0.0))));

	//input_tensor = pad_img(input_tensor);

	//std::cout<<input_tensor.size()<<std::endl;
	//std::cout<<input_tensor[0].size()<<std::endl;
	//std::cout<<input_tensor[0][0].size()<<std::endl;
	//std::cout<<input_tensor[0][0][0].size()<<std::endl;

	for (int i = 0; i < input_height; ++i)
	{
		for (int j = 0; j < input_width; ++j)
		{
			vector4D input_slice;
			for (int k = 0; k < batch_size; k++)
			{
				vector3D input_row;
				for (int n = i*stride; n < i * stride + this->filter_size; n++) 
				{
                	vector2D input_col;
                	for (int p = j * stride; p < j * stride + this->filter_size; p++) 
                	{
                    	vector1D input_channel;
                    	for (int q = 0; q < input_channels; q++) 
                    	{
                        	input_channel.push_back(input_tensor[k][n][p][q]);
                        	
                    	}
                    	input_col.push_back(input_channel);
                    	
                	}
                	input_row.push_back(input_col);
            	}
            	input_slice.push_back(input_row);	
        	}

        	//ispisivanje input slice
        	/*
        	std::cout<<"i = "<<i<<" j = "<<j<<std::endl;
        	for (int k = 0; k < 3; k++)
        	{
        		for (int n = 0; n < 3; n++)
        		{
        			for (int p= 0; p < 3; p++)
        			{
        				std::cout<<input_slice[0][n][p][k]<<" "; 
        			}
        			std::cout<<std::endl;
        		}
        		std::cout<<std::endl;
        	}
        	*/

        	for (int k = 0; k < batch_size; k++)
        	{	
            	for(int n = 0;n<this->num_of_filters;n++)
            	{
            		double conv_sum = 0.0;
            		for(int m=0; m < input_channels;m++)
            		{
            			for (int p = 0; p < this->filter_size ; p++)
            			{	
            				for (int q = 0; q < this->filter_size ; q++)
            				{
            					//std::cout<<conv_sum<<" + " << input_slice[k][p][q][m]<<" * "<<this->conv_filter[p][q][m][n]<<"\t";
            					conv_sum = conv_sum + input_slice[k][p][q][m] * this->conv_filter[p][q][m][n];
            				}
            				//std::cout<<std::endl;
            			}
            			//std::cout<<std::endl;
            			
            		}
            		//std::cout<<"Gotov piksel ";
            		//std::cout<<"i = "<<i<<" j = "<<j<<" kanal = "<<n<<std::endl;

            		//std::cout<<conv_sum<<" + "<<this->bias[n]<<" = "<<conv_sum + this->bias[n]<<std::endl;
            		if(conv_sum + this->bias[n] > 0)
            			this->output[k][i][j][n] = conv_sum + this->bias[n];
            		else
						this->output[k][i][j][n] = 0;
            	}
            }

		}
	}

	return this->output;
}


void ConvLayer::load_weights(const char *filename_weights,const char *filename_bias){
	ifstream file;
	file.open(filename_weights);
	string column;
	int filter=0;
	int channel=0;
	int row=0;
	int t=0;
	while(getline(file,column))
	{
		t=sscanf(column.c_str(),"%f %f %f",&conv_filter[row][0][channel][filter],&conv_filter[row][1][channel][filter],&conv_filter[row][2][channel][filter]);
		//cout<<t<<endl;
		row++;
		if(row == this->filter_size){
			row = 0;
			channel++;
			if(channel == this->num_of_channels){
				channel=0;
				filter++;
				if(filter == this->num_of_filters)
					break;
			}
		}
	}
	file.close();
	row=0;
	file.open(filename_bias);
	while(getline(file,column))
	{
		t=sscanf(column.c_str(),"%f",&bias[row]);
		row++;
		if(row == this->num_of_filters)
			break;
	}
	
}

