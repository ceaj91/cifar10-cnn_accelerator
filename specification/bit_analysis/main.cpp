#include "ConvLayer.hpp"
#include <iostream>
#include <systemc>
#include <fstream>
#include <string>
#include <cstdio>
#include "common.hpp"
#include <iomanip>
#include "../cpp_implementation/MaxPoolLayer.hpp"
#include "../cpp_implementation/flattenlayer.hpp"
#include "../cpp_implementation/denselayer.hpp"
int sc_main(int argc, char* argv[]) {
	//UCITAVANJE SLIKE NE RADI, DOTLE SI STIGAO  "slika.txt"
	//kada ucitas sliku, implementiraj forward_propagation
	vector4D output;
	vector4D output2;
	vector4D output3;
	vector4D output4;
	vector4D output5;
	vector4D output6;
	vector2D output7;
	vector2D output8;
	vector2D output9;

	char  *main_file;
	string line;
	vector<string> files_name;

	vector4D slika(1,vector3D(32,vector2D(32,vector1D(3,0.0))));
	ifstream file_slike;
	ifstream file_labele;
	ifstream input_files;
	ofstream file_info;

	double num;
	int label;
	int index=0;
	float max=0;
	double num_correct=0.0;
	double accuracy;
	int W;
	int F;

	main_file = argv[1]; 
	
	input_files.open(main_file);
	//reading paths of files for pictures, labels, and cnn parametars
	while(getline(input_files,line))
	{
		files_name.push_back(line);
	}
	input_files.close();


	for(int s = 14; s<=19;s++)
	{
		for(int b=3;b<=5;b++)
		{
			W=s;
			F=b;
			num_correct=0;
			accuracy=0;
			ConvLayer conv1(3,3,32,"same",W,F);
			MaxPoolLayer maxpool1(2);
			ConvLayer conv2(3,32,32,"same",W,F);
			MaxPoolLayer maxpool2(2);
			ConvLayer conv3(3,32,64,"same",W,F);
			MaxPoolLayer maxpool3(2);
			FlattenLayer flatten1;
			DenseLayer dense1(1024,512,0);
			DenseLayer dense2(512,10,1);

			conv1.load_weights(files_name[0].c_str(),files_name[1].c_str());
			conv2.load_weights(files_name[2].c_str(),files_name[3].c_str());
			conv3.load_weights(files_name[4].c_str(),files_name[5].c_str());
			dense1.load_dense_layer(files_name[6].c_str(),files_name[7].c_str());
			dense2.load_dense_layer(files_name[8].c_str(),files_name[9].c_str());
			//conv1.print_weights();
			cout<<"Format: "<<F<<"."<<W-F<<endl;
			file_slike.open(files_name[10].c_str());
			file_labele.open(files_name[11].c_str());
			for (int pic_num = 0; pic_num < 10; pic_num++)
			{
				
				file_labele >> label;
				for (int channel = 0; channel < INPUT_CHANNEL_SIZE; channel++)
				{
				
					for (int row = 0; row < INPUT_PICTURE_SIZE; row++)
					{
						for (int column = 0; column < INPUT_PICTURE_SIZE; column++)
						{
							file_slike >> num;
						
							slika[0][row][column][channel] = num/255.0;
						//std::cout<<slika[0][row][column][channel] <<" ";
						}
						//std::cout<<std::endl;
					}
				}

				output = conv1.forward_prop(slika);
				/*for (int i = 0; i < 32; ++i)
				{
					cout<<output[0][0][0][i]<<endl;
				}*/
				output2 = maxpool1.forward_prop(output,{});
				output3 = conv2.forward_prop(output2);
				output4 = maxpool2.forward_prop(output3,{});
				output5 = conv3.forward_prop(output4);
				output6 = maxpool3.forward_prop(output5,{});
				output7 = flatten1.forward_prop(output6);
				output8 = dense1.forward_prop(output7);
				output9 = dense2.forward_prop(output8);
				max=output9[0][0];
				index=0;
				for (int i = 1; i < 10; i++)
				{
					if(output9[0][i] > max){
						max = output9[0][i];
						index=i;
					}
				}
				if(label == index){
					
					num_correct++;					
				}
		
			}

			accuracy = num_correct/10.0 * 100;
			file_info.open("bit_analysis.txt",ios::app);
			file_info<<"Format: "<<F<<"."<<W-F<<" ("<<s<<" bits)"<<" accuracy : "<<accuracy<<"%"<<std::endl;
			std::cout<<"Format: "<<F<<"."<<W-F<<" ("<<s<<" bits)"<<" accuracy : "<<accuracy<<"%"<<std::endl;
			file_info.close();
			file_slike.close();
			file_labele.close();
			
		}
	}




	
	
	return 0;
}
