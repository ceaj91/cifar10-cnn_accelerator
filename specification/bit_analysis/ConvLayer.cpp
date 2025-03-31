#include "ConvLayer.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include "common.hpp"

void copy2fix(vector4D& orig ,vector4D_impl& dest,int W, int F)
{
	impl_t d(W,F);
	for (int i = 0; i < orig.size(); ++i)
	{
		for (int j = 0; j < orig[0].size(); j++)
		{
			for (int k = 0; k < orig[0][0].size(); ++k)
			{
				for (int n = 0; n < orig[0][0][0].size(); ++n)
				{

					d = orig[i][j][k][n];
					dest[i][j][k][n] = d;

					//cout<<"Checking copy2fix"<<endl;
					//cout<<setprecision(12)<<"Double original : "<<orig[i][j][k][n]<<" Fix dest format: "<<dest[i][j][k][n]<<endl;

				}
			}
		}
	}


}

void copy2double(vector4D& orig ,vector4D_impl& dest)
{

for (int i = 0; i < orig.size(); ++i)
	{
		for (int j = 0; j < orig[0].size(); j++)
		{
			for (int k = 0; k < orig[0][0].size(); ++k)
			{
				for (int n = 0; n < orig[0][0][0].size(); ++n)
				{
					orig[i][j][k][n] = dest[i][j][k][n];
					//cout<<"Checking copy2double"<<endl;
					//cout<<setprecision(12)<<"Double orig : "<<orig[i][j][k][n]<<" Fix dest format: "<<setprecision(12)<<dest[i][j][k][n]<<endl;
				}
			}
		}
	}

}

ConvLayer::ConvLayer(int filter_size,int num_of_channels,int num_of_filters,string padding,int W,int F)
{
	this->W=W;
	this->F=F;
	impl_t temp(W,F);
	temp=0;
	this->filter_size = filter_size;
	this->num_of_channels = num_of_channels;
	this->num_of_filters=num_of_filters;
	this->padding = padding;	
	this->conv_filter=vector4D_impl(filter_size, vector3D_impl(filter_size,vector2D_impl(num_of_channels,vector1D_impl(num_of_filters,temp))));
    this->bias = vector1D_impl(num_of_filters,temp);
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
}


vector4D ConvLayer::forward_prop(vector4D input_tensor)
{
	impl_t conv_sum(this->W,this->F);
	impl_t temp(this->W,this->F);
	temp = 0;
	int batch_size = input_tensor.size();
	int input_height = input_tensor[0].size();
	int input_width = input_tensor[0][0].size();
	int input_channels = input_tensor[0][0][0].size();
	int stride =1;
	//uvek ce "same" bit
	this->output = vector4D(batch_size,vector3D(input_height,vector2D(input_width,vector1D(this->num_of_filters,0.0))));

	input_tensor = pad_img(input_tensor);   

	//cout<<input_tensor[0].size()<<" "<< input_height<<endl;
	vector4D_impl input_impl(batch_size,vector3D_impl(input_height+2,vector2D_impl(input_width+2,vector1D_impl(input_channels,temp))));
	vector4D_impl output_impl(batch_size,vector3D_impl(input_height,vector2D_impl(input_width,vector1D_impl(this->num_of_filters,temp))));;

	copy2fix(input_tensor,input_impl,this->W,this->F);
	//copy2fix(input_tensor,input_impl,this->W,this->F);


	//std::cout<<input_tensor.size()<<std::endl;
	//std::cout<<input_tensor[0].size()<<std::endl;
	//std::cout<<input_tensor[0][0].size()<<std::endl;
	//std::cout<<input_tensor[0][0][0].size()<<std::endl;

	for (int i = 0; i < input_height; ++i)
	{
		for (int j = 0; j < input_width; ++j)
		{
			vector4D_impl input_slice;
			for (int k = 0; k < batch_size; k++)
			{
				vector3D_impl input_row;
				for (int n = i*stride; n < i * stride + this->filter_size; n++) 
				{
                	vector2D_impl input_col;
                	for (int p = j * stride; p < j * stride + this->filter_size; p++) 
                	{
                    	vector1D_impl input_channel;
                    	for (int q = 0; q < input_channels; q++) 
                    	{
                        	input_channel.push_back(input_impl[k][n][p][q]);
                        	
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
            		
            		conv_sum=0;
            		for(int m=0; m < input_channels;m++)
            		{
            			for (int p = 0; p < this->filter_size ; p++)
            			{	
            				for (int q = 0; q < this->filter_size ; q++)
            				{
            					//if(p==0 && m == 0 && q==0)
            						//std::cout<<conv_sum<<" + " << input_slice[k][p][q][m]<<" * "<<this->conv_filter[p][q][m][n]<<endl;
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
            			output_impl[k][i][j][n] = conv_sum + this->bias[n];
            		else
						output_impl[k][i][j][n] = 0;
            	}
            }

		}
	}

	copy2double(this->output,output_impl);

	return this->output;
}


void ConvLayer::load_weights(const char *filename_weights,const char *filename_bias){
	impl_t temp(this->W,this->F);
	float p1,p2,p3;
	ifstream file;
	file.open(filename_weights);
	string column;
	int filter=0;
	int channel=0;
	int row=0;
	int t=0;
	//cout<<"------------------------------------------------------"<<endl;
	while(getline(file,column))
	{
		t=sscanf(column.c_str(),"%f %f %f",&p1,&p2,&p3);
		temp = p1;
		conv_filter[row][0][channel][filter] = temp;
		temp = p2;
		conv_filter[row][1][channel][filter] = temp;
		temp = p3;
		conv_filter[row][2][channel][filter] = temp;
		//cout<<"Checking weigts:"<<endl;
		//cout<<setprecision(12)<<"Float format: "<<p3<<"Fix format: "<<setprecision(12)<<temp<<endl;
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
		t=sscanf(column.c_str(),"%f",&p1);
		temp = p1;
		bias[row] = temp;
		row++;
		if(row == this->num_of_filters)
			break;
	}
	
}






