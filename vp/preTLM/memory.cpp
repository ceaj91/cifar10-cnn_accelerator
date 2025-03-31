#ifndef MEM_C
#define MEM_C
#include "types.hpp"
#include "memory.hpp"
#include "addresses.hpp"



Mem::Mem(sc_module_name name, sc_event *do_conv, sc_event *load_param, deque <hwdata_t>& images,deque <hwdata_t>& weigts,deque <hwdata_t>& bias):sc_module(name), do_conv(do_conv), load_param(load_param), images(images), weigts(weigts),bias(bias)																																			
{

	SC_THREAD(grab_from_mem);
	cout<<"Memory constructed"<<endl;
} 

void Mem::grab_from_mem() 
{

	int img_num=0;
	int num_of_img = 10;
	file_extract();
	
		images.clear();
		

		for (int i = PICTURE_1_START ; i <= PICTURE_1_END; ++i)
		{
			images.push_back(ram[i]);
		}

		for (int j = 0; j < 3; ++j)
		{
			wait(load_param[j]);
			weigts.clear();
			bias.clear();
			switch(j)
			{
			case 0:
				for (int i = CONV1_WEIGTS_START ; i <= CONV1_WEIGTS_END; ++i)
				{
					weigts.push_back(ram[i]);
				}
				for (int i = CONV1_BIAS_START ; i <= CONV1_BIAS_END; ++i)
				{
					bias.push_back(ram[i]);
				}
				break;
			case 1:
				for (int i = CONV2_WEIGTS_START ; i <= CONV2_WEIGTS_END; ++i)
				{
					weigts.push_back(ram[i]);
				}
				for (int i = CONV2_BIAS_START ; i <= CONV2_BIAS_END; ++i)
				{
					bias.push_back(ram[i]);
				}
				break;
			case 2:
				for (int i = CONV3_WEIGTS_START ; i <= CONV3_WEIGTS_END; ++i)
				{
					weigts.push_back(ram[i]);
				}
				for (int i = CONV3_BIAS_START ; i <= CONV3_BIAS_END; ++i)
				{
					bias.push_back(ram[i]);
				}
				break;
				
			}
			do_conv[j].notify(SC_ZERO_TIME);
		}

}

void Mem::file_extract()
{
	ifstream file_param;
	string weights = "../../data/parametars/conv";
	string bias = "../../data/parametars/conv";
	char *weights1 ="../../data/parametars/conv1/conv1_filters.txt"; 
	char *bias1 ="../../data/parametars/conv1/conv1_bias.txt"; 
	char *weights2 ="../../data/parametars/conv2/conv2_filters.txt"; 
	char *bias2 ="../../data/parametars/conv2/conv2_bias.txt"; 
	char *weights3 ="../../data/parametars/conv3/conv3_filters.txt"; 
	char *bias3 ="../../data/parametars/conv3/conv3_bias.txt"; 
	char *pictures ="../../data/pictures.txt"; 
	char *lables ="../../data/labels.txt"; 
	int lines;


	file_param.open(weights1);
	lines=num_of_lines(weights1);	
	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 
	
	file_param.open(bias1);
	lines=num_of_lines(bias1);	
	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 
	

	file_param.open(weights2);
	lines=num_of_lines(weights2);	
	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 
	
	file_param.open(bias2);
	lines=num_of_lines(bias2);	
	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 


	file_param.open(weights3);
	lines=num_of_lines(weights3);	
	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 
	
	file_param.open(bias3);
	lines=num_of_lines(bias3);	
	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
	}
	file_param.close(); 

	file_param.open(pictures);
	lines=num_of_lines(pictures);	
	for (int i = 0; i < lines*32; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value/255.0);
	}
	file_param.close();

}

int Mem::num_of_lines(const char* file_name)
{

	int count = 0;
	string line;
	ifstream str_file(file_name);
	if(str_file.is_open())
	{
	  while(getline(str_file,line))
	     count++;
	  str_file.close();
	}
	else
	  cout<<"error opening str file in method num of lines"<<endl;
	return count;
}
#endif