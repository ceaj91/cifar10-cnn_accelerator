#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <vector>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdint>
#include <fstream>
#include <bitset>
#include <cstring>
#include <iomanip>
#include <chrono>
#include <ratio>

#include "../../specification/cpp_implementation/MaxPoolLayer.hpp"
#include "../../specification/cpp_implementation/denselayer.hpp"
#include "../../vp/TLM/addresses.hpp"

#define IP_COMMAND_LOAD_BIAS			0x0001
#define IP_COMMAND_LOAD_WEIGHTS0		0x0002
#define IP_COMMAND_LOAD_CONV0_INPUT		0x0004
#define IP_COMMAND_START_CONV0			0x0008
#define IP_COMMAND_LOAD_WEIGHTS1		0x0010
#define IP_COMMAND_LOAD_CONV1_INPUT		0x0020
#define IP_COMMAND_START_CONV1			0x0040
#define IP_COMMAND_LOAD_WEIGHTS2		0x0080
#define IP_COMMAND_LOAD_CONV2_INPUT		0x0100
#define IP_COMMAND_START_CONV2			0x0200
#define IP_COMMAND_RESET			0x0400
#define IP_COMMAND_READ_CONV0_OUTPUT		0x0800
#define IP_COMMAND_READ_CONV1_OUTPUT		0x1000
#define IP_COMMAND_READ_CONV2_OUTPUT		0x2000

using namespace std;
using namespace chrono;

typedef vector<vector<vector<vector<float>>>> vector4D;
typedef vector<vector<vector<float>>> vector3D;
typedef vector<vector<float>> vector2D;
typedef vector<float> vector1D;

uint16_t input_bias[128];
uint16_t input_weights0[864];
uint16_t input_weights1_0[4608];
uint16_t input_weights1_1[4608];
uint16_t input_weights2_0[4608];
uint16_t input_weights2_1[4608];
uint16_t input_weights2_2[4608];
uint16_t input_weights2_3[4608];
uint16_t input_picture_0[3072];
uint16_t input_picture_1[10368];
uint16_t input_picture_2[3200];


uint16_t castFloatToBin(float t);
float castBinToFloat(uint16_t binaryValue);
void flatten(vector4D source_vector, vector2D &dest_vector, int img_size, int num_of_channels);
void transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels);
void transform_4D_to_1D(vector4D input_vector, vector1D& output_vector, int img_size, int num_of_channels);
vector<int> format_image(vector<int> ram, int img_size, int num_of_channels);
vector<int> pad_img(vector<int> ram, int img_size, int num_of_channels);
void extract_data();

void write_ip(int command);

vector<int> labels;

int main()
{
	vector1D image;
	vector4D image4D;
	vector<int> conv_input;
	vector4D output;
	vector2D dense1_input;
	vector2D dense1_output;
	vector2D dense2_output;
	
	int num_of_pictures = 1;
	int fd;
	int *p;
	uint16_t temp;
	float fl_temp;
	int in_temp;
			
	float max_output;
	int max_index;
	int hit_count = 0;
	int animal_count = 0;

	MaxPoolLayer *maxpool[3];
	DenseLayer *dense_layer[2];
		
	maxpool[0] = new MaxPoolLayer(2);
	maxpool[1] = new MaxPoolLayer(2);
	maxpool[2] = new MaxPoolLayer(2);
	dense_layer[0] = new DenseLayer(1024,512,0);
	dense_layer[1] = new DenseLayer(512,10,1);
	dense_layer[0]->load_dense_layer("../../data/parametars/dense1/dense1_weights.txt", "../../data/parametars/dense1/dense1_bias.txt");
	dense_layer[1]->load_dense_layer("../../data/parametars/dense2/dense2_weights.txt", "../../data/parametars/dense2/dense2_bias.txt");
	
	FILE *input_picture;
	FILE *input;

	/* ------------------------ */
	/* ------Extract data------ */
	/* ------------------------ */

	extract_data();

	/* ------------------------ */
	/* --------Reset IP-------- */
	/* ------------------------ */
	write_ip(IP_COMMAND_RESET);
	
	/* ------------------------ */
	/* Send biases at the start */
	/* ------------------------ */
	
	fd = open("/dev/dma", O_RDWR | O_NDELAY);
	if(fd < 0)
	{
		cout << "[app] Cannot open /dev/dma for write" << endl;
		return -1;
	}
	
	p = (int*)mmap(0, 128*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(p, input_bias, 128*2);
	munmap(p, 128*2);
/*	close(fd);
	if (fd < 0)
	{
		cout << "[app] Cannot close /dev/dma for write" << endl;
		return -1;
	}
*/
	write_ip(IP_COMMAND_LOAD_BIAS);
	//usleep(500);

	
	/* ------------------------ */
	/* -----Classification----- */
	/* ------------------------ */
	
	cout << "[app] Starting classification..." << endl;
	
	input_picture = fopen("../../../CNN_sysC_cpp/slike.txt", "r");
	
	for(int picture = 0; picture < num_of_pictures; picture++)
	{
		/* Exctract picture */
		
		auto start_classification = high_resolution_clock::now();
	
		conv_input.clear();

		for(int i = 0; i < 3072; i++)
		{
			fscanf(input_picture, "%d", &in_temp);
			conv_input.push_back((int)castFloatToBin((float)in_temp/255.0));
		}
		
		conv_input = pad_img(conv_input, CONV1_PICTURE_SIZE, CONV1_NUM_CHANNELS);
		
		conv_input = format_image(conv_input, CONV1_PADDED_PICTURE_SIZE, CONV1_NUM_CHANNELS);
		
		for(int i = 0; i < conv_input.size(); i++) input_picture_0[i] = conv_input[i]; 

		

		/* Reset IP */
		write_ip(IP_COMMAND_RESET);	
		//usleep(10);
		
		/* Send weights0 */
		
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	//auto  = high_resolution_clock::now();

		p = (int*)mmap(0, 864*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights0, 864*2);
		munmap(p, 864*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_WEIGHTS0);
		//usleep(500);
		
		
		/* Send input picture to CONV0 */	
		
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/
		p = (int*)mmap(0, 3468*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_picture_0, 3468*2);
		munmap(p, 3468*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	
		write_ip(IP_COMMAND_LOAD_CONV0_INPUT);
		//usleep(500);
	
		
		
		/* Start CONV0 */
		write_ip(IP_COMMAND_START_CONV0);
		
		//auto conv0_end = high_resolution_clock::now();

		/* Read results */
	
		write_ip(IP_COMMAND_READ_CONV0_OUTPUT);
		//usleep(500);

		fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	
		p = (int *)mmap(0, 32768*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(p == MAP_FAILED) cout << "[app] MAP FAILED" << endl;
		
		image.clear();

		for(int i = 0; i < 32768/2; i++)
		{
			temp = (uint16_t)((uint32_t)*(p+i) & 0x0000ffff);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);

			temp = (uint16_t)(((uint32_t)*(p+i) & 0xffff0000) >> 16);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);
		}

		munmap(p, 32768*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	
	
		
		/* Maxpool for CONV0 output */
		//auto maxpool0_start = high_resolution_clock::now();
		transform_1D_to_4D(image, image4D, CONV1_PICTURE_SIZE, CONV1_NUM_FILTERS);
		output.clear();
		output = maxpool[0]->forward_prop(image4D, {}); 

		transform_4D_to_1D(output, image, CONV1_PICTURE_SIZE/2, CONV1_NUM_FILTERS);
		//auto maxpool0_end = high_resolution_clock::now();

		
		// Transforming vector<float> to vector<int>
		conv_input.clear();
		for (long unsigned int i = 0; i < image.size(); ++i)
		{
			conv_input.push_back(castFloatToBin(image[i]));
		}
		
		conv_input = pad_img(conv_input, CONV2_PICTURE_SIZE, CONV2_NUM_CHANNELS);
		
		conv_input = format_image(conv_input, CONV2_PADDED_PICTURE_SIZE, CONV2_NUM_CHANNELS);
		
		for(int i = 0; i < conv_input.size(); i++) input_picture_1[i] = conv_input[i]; 
		
		
		/* Reset IP */

		
		write_ip(IP_COMMAND_RESET);
		//usleep(10);

		/* Send input picture to CONV1 */
			//auto conv1_start = high_resolution_clock::now();
		
		fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	
		p = (int*)mmap(0, 10368*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_picture_1, 10368*2);
		munmap(p, 10368*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_CONV1_INPUT);
		//usleep(500);


		/* Send 1/2 of weights1 */
	
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights1_0, 4608*2);
		munmap(p, 4608*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_WEIGHTS1);
		//usleep(500);


		/* Start 1/2 of CONV1 */

		write_ip(IP_COMMAND_START_CONV1);
		//usleep(5000);


		/* Send 2/2 of weights1 */
	
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights1_1, 4608*2);
		munmap(p, 4608*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
		
		write_ip(IP_COMMAND_LOAD_WEIGHTS1);
		//usleep(500);


		/* Start 2/2 of CONV1 */

		write_ip(IP_COMMAND_START_CONV1);
		//usleep(5000);
		//auto conv1_end = high_resolution_clock::now();
		
		/* Read results */

		write_ip(IP_COMMAND_READ_CONV1_OUTPUT);
		//usleep(500);
		
		fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	
		p = (int *)mmap(0, 8192*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(p == MAP_FAILED) cout << "MAP FAILED" << endl;
	

		image.clear();

		for(int i = 0; i < 8192/2; i++)
		{
			temp = (uint16_t)((uint32_t)*(p+i) & 0x0000ffff);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);

			temp = (uint16_t)(((uint32_t)*(p+i) & 0xffff0000) >> 16);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);
		}

		munmap(p, 8192*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
		
		
		//auto maxpool1_start = high_resolution_clock::now();

		/* Maxpool for CONV1 output */

		transform_1D_to_4D(image, image4D, CONV2_PICTURE_SIZE, CONV2_NUM_FILTERS);
		output.clear();
		output = maxpool[1]->forward_prop(image4D, {}); 

		transform_4D_to_1D(output, image, CONV2_PICTURE_SIZE/2, CONV2_NUM_FILTERS);
		
		//auto maxpool1_end = high_resolution_clock::now();
		
		// Transforming vector<float> to vector<int>
		conv_input.clear();
		for (long unsigned int i = 0; i < image.size(); ++i)
		{
			conv_input.push_back(castFloatToBin(image[i]));
		}
		
		conv_input = pad_img(conv_input, CONV3_PICTURE_SIZE, CONV3_NUM_CHANNELS);
		
		conv_input = format_image(conv_input, CONV3_PADDED_PICTURE_SIZE, CONV3_NUM_CHANNELS);
	
		for(int i = 0; i < conv_input.size(); i++) input_picture_2[i] = conv_input[i];


		
		/* Reset IP */
		
		write_ip(IP_COMMAND_RESET);
		//usleep(10);


		/* Send input picture to CONV2 */
		//auto conv2_start = high_resolution_clock::now();
			
		fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	
		p = (int*)mmap(0, 3200*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_picture_2, 3200*2);
		munmap(p, 3200*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_CONV2_INPUT);
		//usleep(500);
		

		/* Send 1/4 of weights2 */
	
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights2_0, 4608*2);
		munmap(p, 4608*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_WEIGHTS2);
		//usleep(500);


		/* Start 1/4 of CONV2 */

		write_ip(IP_COMMAND_START_CONV2);
		//usleep(5000);


		/* Send 2/4 of weights2 */
		
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights2_1, 4608*2);
		munmap(p, 4608*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_WEIGHTS2);
		//usleep(500);


		/* Start 2/4 of CONV2 */

		write_ip(IP_COMMAND_START_CONV2);
		//usleep(5000);


		/* Send 3/4 of weights2 */
		
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights2_2, 4608*2);
		munmap(p, 4608*2);
	/*	close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
	*/	
		write_ip(IP_COMMAND_LOAD_WEIGHTS2);
		//usleep(500);


		/* Start 3/4 of CONV2 */

		write_ip(IP_COMMAND_START_CONV2);
		//usleep(5000);


		/* Send 4/4 of weights2 */
	
	/*	fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	*/	
		p = (int*)mmap(0, 4608*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		memcpy(p, input_weights2_3, 4608*2);
		munmap(p, 4608*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
		
		write_ip(IP_COMMAND_LOAD_WEIGHTS2);
		//usleep(500);


		/* Start 4/4 of CONV2 */

		write_ip(IP_COMMAND_START_CONV2);
		//usleep(5000);
		//auto conv2_end = high_resolution_clock::now();
		

		/* Read results */

		write_ip(IP_COMMAND_READ_CONV2_OUTPUT);
		//usleep(500);
			
		fd = open("/dev/dma", O_RDWR | O_NDELAY);
		if(fd < 0)
		{
			cout << "[app] Cannot open /dev/dma for write" << endl;
			return -1;
		}
	
		p = (int *)mmap(0, 4096*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(p == MAP_FAILED) cout << "[app] MAP FAILED" << endl;

		image.clear();

		for(int i = 0; i < 4096/2; i++)
		{
			temp = (uint16_t)((uint32_t)*(p+i) & 0x0000ffff);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);

			temp = (uint16_t)(((uint32_t)*(p+i) & 0xffff0000) >> 16);
			fl_temp = castBinToFloat(temp);
			image.push_back(fl_temp);
		}

		munmap(p, 4096*2);
		close(fd);
		if (fd < 0)
		{
			cout << "[app] Cannot close /dev/dma for write" << endl;
			return -1;
		}
		
	
		/* Maxpool for CONV2 output */
		//auto maxpool2_start = high_resolution_clock::now();
		
		transform_1D_to_4D(image, image4D, CONV3_PICTURE_SIZE, CONV3_NUM_FILTERS);
		output.clear();
		output = maxpool[2]->forward_prop(image4D, {}); 
		//auto maxpool2_end = high_resolution_clock::now();
		
		//auto flatten_start = high_resolution_clock::now();
		
		flatten(output,dense1_input,CONV3_PICTURE_SIZE/2,CONV3_NUM_FILTERS);
		
		//auto flatten_end = high_resolution_clock::now();
		//auto dense1_start = high_resolution_clock::now();
		
		dense1_output=dense_layer[0]->forward_prop(dense1_input);
		//auto dense1_end = high_resolution_clock::now();
		//auto dense2_start = high_resolution_clock::now();
		
		dense2_output=dense_layer[1]->forward_prop(dense1_output);
		//auto dense2_end = high_resolution_clock::now();
		
		//auto end_classification = high_resolution_clock::now();
		max_output = dense2_output[0][0];
		max_index = 0;

		

		for (int i = 0; i < 10; ++i)
		{
			cout << dense2_output[0][i] << endl;
			if(dense2_output[0][i] > max_output)
			{
				max_output = dense2_output[0][i];
				max_index = i;
			}
		}

		//auto classification_duratin =  duration_cast<milliseconds>(end_classification - start_classification);
		//auto conv0_duration = duration_cast<milliseconds>(conv0_start - conv0_end);
		//auto conv1_duration = duration_cast<milliseconds>(conv1_start - conv1_end);
		//auto conv2_duration = duration_cast<milliseconds>(conv2_start - conv2_end);
		//auto maxpool0_duration = duration_cast<milliseconds>(maxpool0_start - maxpool0_end);
		//auto maxpool1_duration = duration_cast<milliseconds>(maxpool1_start - maxpool1_end);
		//auto maxpool2_duration = duration_cast<milliseconds>(maxpool2_start - maxpool2_end);
		//auto flatten_duration = duration_cast<microseconds>(flatten_start - flatten_end);
		//auto dense1_duration = duration_cast<milliseconds>(dense1_start - dense1_end);
		//auto dense2_duration = duration_cast<microseconds>(dense2_start - dense2_end);


		//cout << "Entire picture " << picture << " took " << classification_duratin.count() << "ms" << endl;
		//cout << "CONV0 took " << conv0_duration.count() << "ms" << endl;
		//cout << "Maxpool0 took " << maxpool0_duration.count() << "ms" << endl;
		//cout << "CONV1 took " << conv1_duration.count() << "ms" << endl;
		//cout << "Maxpool1 took " << maxpool1_duration.count() << "ms" << endl;
		//cout << "CONV2 took " << conv2_duration.count() << "ms" << endl;
		//cout << "Maxpool2 took " << maxpool2_duration.count() << "ms" << endl;
		//cout << "Flatten took " << flatten_duration.count() << "ms" << endl;
		//cout << "Dense1 took " << dense1_duration.count() << "ms" << endl;
		//cout << "Dense2 took " << dense2_duration.count() << "ms" << endl;

		if(labels[picture] == 2 ||
			labels[picture] == 3 ||
			labels[picture] == 4 ||
			labels[picture] == 5 ||
			labels[picture] == 6 ||
			labels[picture] == 7
		  )
		{
			animal_count++;
			if(labels[picture] == max_index)
			{
				cout << "[app] Picture " << picture << " -  HIT!" << endl;
				hit_count++;
			}
			else
			{
				cout << "[app] Picture " << picture << " -  MISS!" << endl;
			}
			if(picture % 100)
			{
				cout << picture << " classified" << endl;
			}
		}
	}
	fclose(input_picture);
	
	cout << endl << endl << "[app] Number of hits: " << hit_count << endl;
	cout << "[app] Animal count: " << animal_count << endl;
	cout << "[app] Network accuracy is " << (float)hit_count*100.0/animal_count << "%" << endl;
	
	return 0;
}


void write_ip(int command)
{
	// cout << "[app] Inside write_ip for " << hex << command << endl;
	
	int ret;
	FILE *cnn_file;
	
	cnn_file = fopen("/dev/cnn-ip", "w");
	
	// cout << "[app] Opened cnn-ip for " << hex << command << endl;
	
	if(cnn_file == NULL) cout << "[app] Could not open /dev/cnn-ip" << endl;
	// else cout << "[app] Opened /dev/cnn-ip" << endl;
	
	// auto start_write = high_resolution_clock::now();
	ret = fprintf(cnn_file, "%d\n", command);
	
	// auto stop_write = high_resolution_clock::now();
	// auto elapsed = duration_cast<microseconds>(stop_write - start_write);

	// cout << "[app] After fprintf returned " << ret << " for " << hex << command << endl;
	// cout << "[app] Elapsed time " << elapsed.count() << endl;

	if(fclose(cnn_file)) cout << "[app] Cannot close /dev/cnn-ip" << endl;
	// else cout << "[app] Successfully closed /dev/cnn-ip after " << hex << command << endl << endl <<  endl;
}

void flatten(vector4D source_vector,vector2D &dest_vector,int img_size, int num_of_channels)
{
	dest_vector.clear();
	vector1D tmp;
	for (int row = 0; row < img_size; ++row)
	{	
		for (int column = 0; column < img_size; ++column)
		{
			for (int channel = 0; channel < num_of_channels; ++channel)
			{
				tmp.push_back(source_vector[0][row][column][channel]);
			}
		}
	}
	dest_vector.push_back(tmp);
}

void transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels)
{
	output_vector.clear();
	vector3D rows;
	for (int row = 0; row < img_size; ++row)
	{	
		vector2D columns;
		for (int column = 0; column < img_size; ++column)
		{
			vector1D channels; 
			for (int channel = 0; channel < num_of_channels; ++channel)
			{
				channels.push_back(input_vector[channel * img_size*img_size + column + row * img_size]);
			}
			columns.push_back(channels);
		}
		rows.push_back(columns);
	}
	output_vector.push_back(rows);
}

void transform_4D_to_1D(vector4D source_vector, vector1D& dest_vector,int img_size, int num_of_channels)
{
	dest_vector.clear();
	for (int channel = 0; channel < num_of_channels; ++channel)
	{	
		for (int row = 0; row < img_size; ++row)
		{
			for (int column = 0; column < img_size; ++column)
			{
				dest_vector.push_back(source_vector[0][row][column][channel]);
			}
		}
	}
}

vector<int> pad_img(vector<int> ram, int img_size, int num_of_channels)
{
	for(int channel = 0 ; channel < num_of_channels; channel++)
    	{
			// Firstly, zeros are emplaced for the first padded row (image_size(one row) + 2 for the edges)
	        for (int i = 0; i < img_size+2; i++)
	        {
	        	ram.emplace((ram.begin() + (channel)*(img_size+2)*(img_size+2) + i), 0);
	        }

			// Secondly, zeros are added to each row's edge
	        for(int rows = 1; rows < img_size + 1; rows++)
	        {
				// pos1 calulates the position to insert the left-most zero in each row (left edge)
				// Component "(channel)*(img_size+2)*(img_size+2)" refers to the size of the channel
				// Component "rows*img_size" refers to the number of rows that have been padded on the current channel
				// Component "rows*2" takes into account the number of edge pixels that have been added (padded) on the current channel
	        	int pos1 = (channel)*(img_size+2)*(img_size+2) + rows*img_size + rows*2;
				// pos2 calulates the position to insert the right-most zero in each row (right edge)
	        	int pos2 = (channel)*(img_size+2)*(img_size+2) + rows*img_size + rows*2 + 1 + img_size;
	        	ram.emplace((ram.begin() + pos1), 0);
	        	ram.emplace((ram.begin() + pos2), 0);
	        }

			// Finally, zeros are pushed back as we fill up the final row of the padded image (plus 2 for edges)
	        for (int i = 0; i < img_size + 2; i++)
	        {
	        	ram.emplace((ram.begin() + ((channel)*(img_size+2)*(img_size+2)) + (img_size+2)*(img_size+1) + i), 0);
	        }
    	}
    	
    	return ram;
}

vector<int> format_image(vector<int> ram, int img_size, int num_of_channels)
{
	vector <int> temp_ram;

	temp_ram.clear();
	
	for(int i = 0; i < img_size; i++)
	{
		for(int j = 0; j < num_of_channels; j++)
		{
			for(int k = 0; k < 3; k++)
			{
				temp_ram.push_back(ram[i + j * img_size * img_size + k * img_size]);
			}
		}
	}
	
	for(int i = 3; i < img_size; i++)
	{
		for(int j = 0; j < img_size; j++)
		{
			for(int k = 0; k < num_of_channels; k++)
			{
				temp_ram.push_back(ram[j + k * img_size * img_size + i * img_size]);
			}
		}
	}

	ram.clear();
	//ofstream results_file;
	//results_file.open("../../data/picture1_formated.txt");
	for(int i = 0; i < temp_ram.size(); i++) 
	{
		ram.push_back(temp_ram[i]);
		//results_file << ram[ram.size() - 1] << endl;


	}

	//results_file.close();
	return ram;
}

uint16_t castFloatToBin(float t) 
{
	int sign = (t >= 0) ? 0 : 1;
	float resolution = 0.000244140625;
	float half_of_resolution = 0.0001220703125;
	int deo;
	int integerPart;
	int decimalPart;
	uint16_t binaryValue;
    
	if(sign == 0)
	{
	deo = t/resolution;
	if(t >= deo*resolution+half_of_resolution)
	    deo++;
	 binaryValue=deo;

	}
	else
	{
	 deo = t/resolution*(-1);
	 if(t <= (-1)*deo*resolution-half_of_resolution)
	     deo++;
	 binaryValue = 65536-deo;
	}

	return binaryValue;
}

float castBinToFloat(uint16_t binaryValue) 
{
	uint16_t binaryValue_uint = binaryValue;
	int sign = (binaryValue_uint >> 15) & 0x1;

	if (sign == 1) {
	binaryValue_uint = (~binaryValue_uint) + 1; // prebacujemo u pozitivno, posle cemo float pomnozitit sa -1
	}

	int integerPart = (binaryValue_uint >> 12) & 0x7;
	int decimalPart = binaryValue_uint & 0xFFF;

	float floatValue = (float)integerPart + ((float)decimalPart / 4096.0f);
	if (sign == 1)
	floatValue = floatValue * (-1);

	return floatValue;
}


void extract_data()
{
	float temp;
	int in_temp;
	uint16_t res;
	FILE *input;

	// Extracting bias

	input = fopen("../../data/conv0_input/bias_formated.txt", "r");

	for(int i = 0; i < 128; i++)
	{
		fscanf(input, "%f", &temp);
		input_bias[i] = castFloatToBin(temp);
	}
	fclose(input);
	
	// Extracting weights0
	
	input = fopen("../../data/conv0_input/weights0_formated.txt", "r");
	for(int i = 0; i < 864; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights0[i] = castFloatToBin(temp);
	}
	fclose(input);

	// Extracting weights1

	input = fopen("../../data/conv1_input/weights1_formated.txt", "r");
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights1_0[i] = castFloatToBin(temp);
	}
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights1_1[i] = castFloatToBin(temp);
	}
	fclose(input);

	// Extracting weights2
	
	input = fopen("../../data/conv2_input/weights2_formated.txt", "r");
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights2_0[i] = castFloatToBin(temp);
	}
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights2_1[i] = castFloatToBin(temp);
	}
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights2_2[i] = castFloatToBin(temp);
	}
	for(int i = 0; i < 4608; i++)
	{
		fscanf(input, "%f", &temp);
		input_weights2_3[i] = castFloatToBin(temp);
	}
	fclose(input);
	
	// Extracting labels
	
	input = fopen("../../../CNN_sysC_cpp/labele.txt", "r");
	for(int i = 0; i < 10000; i++)
	{
		fscanf(input, "%d", &in_temp);
		labels.push_back(in_temp);
	}
	fclose(input);	
}
