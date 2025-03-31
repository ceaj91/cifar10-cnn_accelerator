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
int main() {
	//UCITAVANJE SLIKE NE RADI, DOTLE SI STIGAO  "slika.txt"
	//kada ucitas sliku, implementiraj forward_propagation
	vector4D output;
	vector4D output2;
	vector4D output3;
	vector4D output4;
	vector4D output5;
	vector4D output6;
	vector2D output7;
	vector1D output8;
	vector1D output9;

	vector4D slika(1,vector3D(32,vector2D(32,vector1D(3,0.0))));
	ifstream file;
	file.open("slika.txt");
	double num;

	//ucitavanje slike
	for (int channel = 0; channel < INPUT_CHANNEL_SIZE; channel++)
	{
		
		for (int row = 0; row < INPUT_PICTURE_SIZE; row++)
		{
			for (int column = 0; column < INPUT_PICTURE_SIZE; column++)
			{
				file >> num;
				
				slika[0][row][column][channel] = num/255.0;
				//std::cout<<slika[0][row][column][channel] <<" ";
			}
			//std::cout<<std::endl;
		}
	}
 
	const char *weights1 = "../../CNN-CIFAR10/parametars/conv1/conv1_filters.txt";
	const char *bias1 = "../../CNN-CIFAR10/parametars/conv1/conv1_bias.txt";
	const char *weights2 = "../../CNN-CIFAR10/parametars/conv2/conv2_filters.txt";
	const char *bias2 = "../../CNN-CIFAR10/parametars/conv2/conv2_bias.txt";
	const char *weights3 = "../../CNN-CIFAR10/parametars/conv3/conv3_filters.txt";
	const char *bias3 = "../../CNN-CIFAR10/parametars/conv3/conv3_bias.txt";
	const char *dense1_weights = "../../CNN-CIFAR10/parametars/dense1/dense1_weights.txt";
	const char *dense1_bias = "../../CNN-CIFAR10/parametars/dense1/dense1_bias.txt";
	const char *dense2_weights = "../../CNN-CIFAR10/parametars/dense2/dense2_weights.txt";
	const char *dense2_bias = "../../CNN-CIFAR10/parametars/dense2/dense2_bias.txt";
	ConvLayer conv1(3,3,32);
	MaxPoolLayer maxpool1(2);
	ConvLayer conv2(3,32,32);
	MaxPoolLayer maxpool2(2);
	ConvLayer conv3(3,32,64);
	MaxPoolLayer maxpool3(2);
	FlattenLayer flatten1;
	DenseLayer dense1(1024,512,0);
	DenseLayer dense2(512,10,1);

	conv1.load_weights(weights1,bias1);
	conv2.load_weights(weights2,bias2);
	conv3.load_weights(weights3,bias3);
	dense1.load_dense_layer(dense1_weights,dense1_bias);
	dense2.load_dense_layer(dense2_weights,dense2_bias);

	output = conv1.forward_prop(slika);
	output2 = maxpool1.forward_prop(output,{});
	output3 = conv2.forward_prop(output2);
	output4 = maxpool2.forward_prop(output3,{});
	output5 = conv3.forward_prop(output4);
	output6 = maxpool3.forward_prop(output5,{});
	output7 = flatten1.forward_prop(output6);
	output8 = dense1.forward_prop(output7[0]);
	output9 = dense2.forward_prop(output8);
	for (int i = 0; i < 10; i++)
	{
		
		std::cout<<std::setprecision(18)<<output9[i]<<std::endl;

	}



	/*
	for (int channel = 0; channel < INPUT_CHANNEL_SIZE ; channel++)
	{
		
		for (int row = 0; row < INPUT_PICTURE_SIZE ; row++)
		{
			for (int column = 0; column < INPUT_PICTURE_SIZE ; column++)
			{
				
				std::cout<<output[0][row][column][channel] <<" ";
			}
			std::cout<<std::endl;
		}
	}
	*/
	//conv1.print_weights();
	//output=conv1.forward_prop(slika);
	/*for (int i = 0; i < 32; i++)
	{
		std::cout<<std::setprecision(20)<<output[0][30][i][30]<<std::endl;
	}
*/

	
	
	return 0;
}