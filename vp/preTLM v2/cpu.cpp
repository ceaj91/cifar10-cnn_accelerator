#ifndef CPU_C
#define CPU_C
#include "cpu.hpp"

Cpu::Cpu(sc_module_name name, sc_event *load_image, sc_event* do_dense, deque <hwdata_t>& images):sc_module(name), load_image(load_image),
																									do_dense(do_dense),
																									images(images)
{
	cout<<"Cpu constructed"<<endl;
	//maxpool[0]=new MaxPoolLayer(2);
	//maxpool[1]=new MaxPoolLayer(2);
	//maxpool[2]=new MaxPoolLayer(2);
	//cout<<"max constructed"<<endl;
	dense_layer[0] = new DenseLayer(1024,512,0);
	dense_layer[1] = new DenseLayer(512,10,1);
	cout<<"Dense constructed"<<endl;
	dense_layer[0]->load_dense_layer("../../data/parametars/dense1/dense1_weights.txt","../../data/parametars/dense1/dense1_bias.txt");
	dense_layer[1]->load_dense_layer("../../data/parametars/dense2/dense2_weights.txt","../../data/parametars/dense2/dense2_bias.txt");
	cout<<"Loaded Dense weights"<<endl;
	SC_THREAD(software);

}
	
void Cpu::software()
{
	//load weights for dense layer
	vector1D temp;

	vector4D output1;
	vector4D output2;
	vector4D output0;
	vector2D dense1_input;
	vector2D dense1_output;
	vector2D dense2_output;

	(load_image[0]).notify(SC_ZERO_TIME);

	// Naredni blok komentar se vise NE IZVRSAVA u softwareu

	/*
	wait(do_maxpool[0]);
	//convert to 4D
	transform_1D_to_4D(32,32);
	//do_maxpool
	output0=maxpool[0]->forward_prop(images_4D, {});
	//convert to 1D
	transform_4D_to_1D(output0,16,32);

	load_param[1].notify(SC_ZERO_TIME);
	wait(do_maxpool[1]);
	
	//convert to 4D
	transform_1D_to_4D(16,32);

	//do_maxpool
	output1=maxpool[1]->forward_prop(images_4D, {});
	
	//convert to 1D
	transform_4D_to_1D(output1,8,32);


	load_param[2].notify(SC_ZERO_TIME);

	wait(do_maxpool[2]);
	

	//convert to 4D
	transform_1D_to_4D(8,64);

	//do_maxpool
	output2=maxpool[2]->forward_prop(images_4D, {});

	//convert to 1D
	*/

	wait(*do_dense);

	// Ovaj sloj je potreban kako bi formirali 1D vector koji odgovara formatu koji dense ocekuje na ulazu
	// Odnosno ne-ljudskom obliku zamisljana matrice
	// Prvo su mu potrebne prve celije od svakog kanala, zatim druge celije od svakog kanal itd.
	flatten(4,64);

	temp.clear();
	for (int i = 0; i < images.size(); ++i)
	{
		temp.push_back(images[i]);
	}
	
	dense1_input.push_back(temp);

	//do dense 1
	dense1_output=dense_layer[0]->forward_prop(dense1_input);


	//do dense 2
	dense2_output=dense_layer[1]->forward_prop(dense1_output);

	//ispis
	
	for (int i = 0; i < 10; ++i)
	{
		cout<<dense2_output[0][i]<<endl;
	}


}																								

void Cpu::flatten(int img_size, int num_of_channels)
{
	deque<hwdata_t> output;
	output.clear();
	for (int row = 0; row < img_size; ++row)
	{	
		for (int column = 0; column < img_size; ++column)
		{
			for (int channel = 0; channel < num_of_channels; ++channel)
			{
				output.push_back(images[channel*img_size*img_size + column + row * img_size]);
			}
		}
	}
	images = output;
}

#endif