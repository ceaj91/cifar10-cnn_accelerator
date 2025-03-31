#include "ConvLayer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "common.hpp"
#include <iomanip>
#include "MaxPoolLayer.hpp"
#include "flattenlayer.hpp"
#include "denselayer.hpp"
#include <chrono>
//enum CIFAR10 {AIRPLANE,AUTOMOBILE,BIRD,CAT,DEER,DOG,FROG,HORSE,SHIP,TRUCK};
using namespace chrono;
vector4D pad_img(const vector4D& input_tensor);




int main(int argc, char *argv[]) {
	
	vector4D output;
	vector4D output2;
	vector4D output3;
	vector4D output4;
	vector4D output5;
	vector4D output6;
	vector4D padd1;
	vector4D padd2;
	vector4D padd3;
	vector2D output7;
	vector2D output8;
	vector2D output9;

	vector4D picture(1,vector3D(32,vector2D(32,vector1D(3,0.0))));
	ifstream file_slike;
	ifstream file_labele;
	ifstream input_files;

	char  *main_file;
	std::vector<string> files_name;

	double num;
	string line;
	int label;
	int index=0;
	float max=0;

	main_file = argv[1]; 
	
	input_files.open(main_file);
	//reading paths of files for pictures, labels, and cnn parametars
	while(getline(input_files,line))
	{
		files_name.push_back(line);
	}
	input_files.close();
	

	ConvLayer conv1(3,3,32);
	MaxPoolLayer maxpool1(2);
	ConvLayer conv2(3,32,32);
	MaxPoolLayer maxpool2(2);
	ConvLayer conv3(3,32,64);
	MaxPoolLayer maxpool3(2);
	FlattenLayer flatten1;
	DenseLayer dense1(1024,512,0);
	DenseLayer dense2(512,10,1);

	conv1.load_weights(files_name[0].c_str(),files_name[1].c_str());
	conv2.load_weights(files_name[2].c_str(),files_name[3].c_str());
	conv3.load_weights(files_name[4].c_str(),files_name[5].c_str());
	dense1.load_dense_layer(files_name[6].c_str(),files_name[7].c_str());
	dense2.load_dense_layer(files_name[8].c_str(),files_name[9].c_str());
	file_slike.open(files_name[10].c_str());
	file_labele.open(files_name[11].c_str());
	for (int pic_num = 0; pic_num < 10; pic_num++)
	{
		auto start = high_resolution_clock::now(); 
		file_labele >> label;
		for (int channel = 0; channel < INPUT_CHANNEL_SIZE; channel++)
		{
		
			for (int row = 0; row < INPUT_PICTURE_SIZE; row++)
			{
				for (int column = 0; column < INPUT_PICTURE_SIZE; column++)
				{
					file_slike >> num;
				
					picture[0][row][column][channel] = num/255.0;
				}
		
			}
		}
		//padd
		padd1 = pad_img(picture);
		auto start_conv0 = high_resolution_clock::now();
		output = conv1.forward_prop(padd1);
		auto stop_conv0 = high_resolution_clock::now();
		output2 = maxpool1.forward_prop(output,{});
		auto stop_maxpool0 = high_resolution_clock::now();
		//padd
		padd2 = pad_img(output2);
		auto start_conv1 = high_resolution_clock::now();
		output3 = conv2.forward_prop(padd2);
		auto stop_conv1 = high_resolution_clock::now();
		output4 = maxpool2.forward_prop(output3,{});
		auto stop_maxpool1 = high_resolution_clock::now();
		//padd
		padd3 = pad_img(output4);
		auto start_conv2 = high_resolution_clock::now();
		output5 = conv3.forward_prop(padd3);
		auto stop_conv2 = high_resolution_clock::now();
		output6 = maxpool3.forward_prop(output5,{});
		auto stop_maxpool2 = high_resolution_clock::now();
		output7 = flatten1.forward_prop(output6);
		auto stop_flatten = high_resolution_clock::now();
		output8 = dense1.forward_prop(output7);
		auto stop_dense1 = high_resolution_clock::now();
		output9 = dense2.forward_prop(output8);
		max=output9[0][0];
		auto stop = high_resolution_clock::now();
		auto elapsed = duration_cast<milliseconds>(stop - start);
		cout << "Time elapsed:\nFor entire picture: " << elapsed.count() << "ms" << endl;
		elapsed = duration_cast<milliseconds>(stop_conv0 - start_conv0);
		cout << "CONV0 took: " << elapsed.count() << "ms" << endl;
		
		elapsed = duration_cast<milliseconds>(stop_maxpool0 - stop_conv0);
		cout << "Maxpool0 took: " << elapsed.count() << "ms" << endl;
	
		elapsed = duration_cast<milliseconds>(stop_conv1 - start_conv1);
		cout << "CONV1 took: " << elapsed.count() << "ms" << endl;

		elapsed = duration_cast<milliseconds>(stop_maxpool1 - stop_conv1);
		cout << "Maxpool1 took: " << elapsed.count() << "ms" << endl;

		elapsed = duration_cast<milliseconds>(stop_conv2 - start_conv2);
		cout << "CONV2 took: " << elapsed.count() << "ms" << endl;

		elapsed = duration_cast<milliseconds>(stop_maxpool2 - stop_conv2);
		cout << "Maxpool2 took: " << elapsed.count() << "ms" << endl;

		elapsed = duration_cast<milliseconds>(stop_flatten - stop_maxpool2);
		cout << "Flatten took: " << elapsed.count() << "ms" << endl;

		elapsed = duration_cast<milliseconds>(stop_dense1 - stop_flatten);
		cout << "Dense1 took: " << elapsed.count() << "ms" << endl;


		elapsed = duration_cast<milliseconds>(stop - stop_dense1);
		cout << "Dense2 took: " << elapsed.count() << "ms" << endl;
		
		
		index=0;
		for (int i = 1; i < 10; i++)
		{
			if(output9[0][i] > max){
				max = output9[0][i];
				index=i;
			}
		}
		cout<<"Picture : ";
		switch(label)
	{
		case 0:
			cout<<"airplane";
			break;
		case 1:
			cout<<"automobile";
			break;
		case 2:
			cout<<"bird";
			break;
		case 3:
			cout<<"cat";
			break;
		case 4:
			cout<<"deer";
			break;
		case 5:
			cout<<"dog";
			break;
		case 6:
			cout<<"frog";
			break;
		case 7:
			cout<<"horse";
			break;
		case 8:
			cout<<"ship";
			break;
		case 9:
			cout<<"truck";
			break;
		default:
			cout<<"NULL";
			break;				
	}
		
		cout<<" Prediction : ";
		switch(index)
	{
		case 0:
			cout<<"airplane"<<endl;
			break;
		case 1:
			cout<<"automobile"<<endl;
			break;
		case 2:
			cout<<"bird"<<endl;
			break;
		case 3:
			cout<<"cat"<<endl;
			break;
		case 4:
			cout<<"deer"<<endl;
			break;
		case 5:
			cout<<"dog"<<endl;
			break;
		case 6:
			cout<<"frog"<<endl;
			break;
		case 7:
			cout<<"horse"<<endl;
			break;
		case 8:
			cout<<"ship"<<endl;
			break;
		case 9:
			cout<<"truck"<<endl;
			break;
		default:
			cout<<"NULL"<<endl;
			break;				
	}



		if(label == index){
			std::cout<<pic_num<<". CORRECT!"<<std::endl;
		}
		else
			std::cout<<pic_num<<". WRONG!"<<std::endl;
	}
	
	file_slike.close();
	file_labele.close();
	return 0;
}


vector4D pad_img(const vector4D& input_tensor){

	
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
