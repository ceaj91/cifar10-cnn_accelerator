#ifndef MEM_C
#define MEM_C

#include "memory.hpp"




Mem::Mem(sc_module_name name, sc_event *do_conv, sc_event *load_image, deque <hwdata_t>& images, deque <hwdata_t>& weigts0, deque <hwdata_t>& weigts1, deque <hwdata_t>& weigts2,
			deque <hwdata_t>& bias0, deque <hwdata_t>& bias1, deque <hwdata_t>& bias2):sc_module(name), do_conv(do_conv), load_image(load_image), images(images), weigts0(weigts0), weigts1(weigts1), weigts2(weigts2),
					bias0(bias0), bias1(bias1), bias2(bias2)																																		
{

	SC_THREAD(grab_from_mem);
	cout<<"Memory constructed"<<endl;
} 

void Mem::grab_from_mem() 
{
	int img_num=0;
	int num_of_img = 10;
	file_extract();

	// Sada na pocetku loadujemo sve paramtere u memoriju i to vise nikada ne radimo
	// S toga nema potrebe za for petljom niti za bilo kakvim eventom vezanim za load_param
	// Nakon elaboracije (pre while(1) petlje tj loadovanja parametara) jedina stvar koja se mijenja
	// U memoriji jeste images vector (razlicite slike)
	// ############## NOTE: za ovaj nivo apstrakcije radimo samo sa jednom slikom

	//for (int j = 0; j < 3; ++j)
	//{
		//wait(*load_param);
		weigts0.clear();
		weigts1.clear();
		weigts2.clear();
		bias0.clear();
		bias1.clear();
		bias2.clear();
		//switch(j)
		//{
		//case 0:
			for (int i = CONV1_WEIGTS_START ; i <= CONV1_WEIGTS_END; ++i)
			{
				weigts0.push_back(ram[i]);
			}
			for (int i = CONV1_BIAS_START ; i <= CONV1_BIAS_END; ++i)
			{
				bias0.push_back(ram[i]);
			}
			//break;
		//case 1:
			for (int i = CONV2_WEIGTS_START ; i <= CONV2_WEIGTS_END; ++i)
			{
				weigts1.push_back(ram[i]);
			}
			for (int i = CONV2_BIAS_START ; i <= CONV2_BIAS_END; ++i)
			{
				bias1.push_back(ram[i]);
			}
			//break;
		//case 2:
			for (int i = CONV3_WEIGTS_START ; i <= CONV3_WEIGTS_END; ++i)
			{
				weigts2.push_back(ram[i]);
			}
			for (int i = CONV3_BIAS_START ; i <= CONV3_BIAS_END; ++i)
			{
				bias2.push_back(ram[i]);
			}
			//break;
			
		//}
		// do_conv.notify(SC_ZERO_TIME);
	//}

	//while(1)
	//{
		// U stvarnoj implementaciji u ovoj beskonacnoj petlji CPU bi pripremao nove slike
		// Koje se loaduju u BRAM svaki put kada dodje od do_dense odnosno do_conv[0] notifikacije
		wait(load_image[0]);

		images.clear();
		for (int i = PICTURE_1_START ; i <= PICTURE_1_END; ++i)
		{
			images.push_back(ram[i]);
		}
		
		do_conv[0].notify(SC_ZERO_TIME);
	//}

}

void Mem::file_extract()
{
	ifstream file_param;
	//string weights = "../../data/parametars/conv";
	//string bias = "../../data/parametars/conv";
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

	if(ram.size() != 0) cout << "Files extracted!" << endl;

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