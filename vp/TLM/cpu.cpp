#ifndef CPU_C
#define CPU_C
#include "cpu.hpp"

Cpu::Cpu(sc_module_name name, char* image_file_name, char* labels_file_name):sc_module(name), ram(ram), image_file_name(image_file_name), labels_file_name(labels_file_name)
{
	
	maxpool[0]=new MaxPoolLayer(2);
	maxpool[1]=new MaxPoolLayer(2);
	maxpool[2]=new MaxPoolLayer(2);
	dense_layer[0] = new DenseLayer(1024,512,0);
	dense_layer[1] = new DenseLayer(512,10,1);
	dense_layer[0]->load_dense_layer("../../data/parametars/dense1/dense1_weights.txt","../../data/parametars/dense1/dense1_bias.txt");
	dense_layer[1]->load_dense_layer("../../data/parametars/dense2/dense2_weights.txt","../../data/parametars/dense2/dense2_bias.txt");
	SC_THREAD(software);
	ip_command=0b000000;
	p_port0.bind(sig0);
	sig0=SC_LOGIC_0;
	labels.clear();
	//cout<<"Cpu constructed"<<endl;
}

void Cpu::flatten(vector4D source_vector,vector2D &dest_vector,int img_size, int num_of_channels)
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

void Cpu::transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels)
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

void Cpu::transform_4D_to_1D(vector4D source_vector,vector1D& dest_vector,int img_size, int num_of_channels)
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

	
void Cpu::software()
{
	//cout << "CPU proc" << endl;
	vector1D image;
	vector4D image4D;
	vector<hwdata_t> orig_img;
	vector4D output;
	vector2D dense1_input;
	vector2D dense1_output;
	vector2D dense2_output;
	unsigned char *buf;
	sc_time start_time;
	sc_time convolution_time;
	sc_time transaction_time;
	sc_time time_start;
	sc_time time_finish;
	sc_time conv0_time;
	sc_time conv1_time;
	sc_time conv2_time;

	ofstream results_file;
	//results_file.open("../../data/vp_pic1_results.txt");

	int hit_number = 0;

	pl_t pl;
	sc_time offset=SC_ZERO_TIME;
	convolution_time = SC_ZERO_TIME;
	transaction_time = SC_ZERO_TIME;
	conv0_time = SC_ZERO_TIME;
	conv1_time = SC_ZERO_TIME;
	conv2_time = SC_ZERO_TIME;

	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif


	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//-------------------------------------------------------------------------------------
	// Load and extract weights and biases from files

	// Assumption: It does not take simulation time to do this
	extract_parameters();

	//-------------------------------------------------------------------------------------
	// Transfer extracted data to Memory component

	buf=(unsigned char*)&ram[0];
	pl.set_address(0);
	pl.set_data_length(WEIGHTS_NUM_OF_PARAMETARS+BIAS_NUM_OF_PARAMETARS);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i1->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	#ifdef PRINTS
	cout<<"Weights and bias parameters stored in memory at: " <<sc_time_stamp() << endl; 
	#endif

	ram.clear();

	//-------------------------------------------------------------------------------------
	//START IP to load bias

	// Biases are loaded into IP (HW's BRAM cell) only once - at the start of simulation
	// Unlike weights, biases are stored and kept in BRAM forever.

	ip_command = 0b00001;
	buf=(unsigned char*)&ip_command;

	pl.set_address(0x80000000);
	pl.set_data_length(1);
	pl.set_command(TLM_WRITE_COMMAND);
	pl.set_data_ptr(buf);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);

	#ifdef QUANTUM
	qk.inc(sc_time(CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#else
	offset += sc_time(CLK_PERIOD, SC_NS);
	#endif

	//-------------------------------------------------------------------------------------
	// SEND BIAS - TROUGH DMA TO IP BLOCK

	time_start = sc_time_stamp();

	// Simulating 40 CLK period delay before transaction
	#ifdef QUANTUM
	qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
	offset = qk.get_local_time();
	qk.set_and_sync(offset);
	#endif

	pl.set_address(0x81000000);
	pl.set_data_length(BIAS_NUM_OF_PARAMETARS);
	pl.set_command(TLM_WRITE_COMMAND);
	s_cp_i0->b_transport(pl, offset);
	assert(pl.get_response_status() == TLM_OK_RESPONSE);
	qk.set_and_sync(offset);

	do
	{
		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif
		tmp_sig=sig0.read();
	} while(tmp_sig == SC_LOGIC_0);

	time_finish = sc_time_stamp();
	transaction_time += time_finish - time_start;

	#ifdef PRINTS
	cout << "Biases loaded into BRAM at: " << sc_time_stamp() << endl << endl;
	#endif


	//-------------------------------------------------------------------------------------
	// Extracting labels in case of testing
	extract_labels();

	//-------------------------------------------------------------------------------------
	// Main loop for multiple pictures

	for(int picture = 0; picture < NUM_OF_PICTURES; picture++)
	{
		start_time = sc_time_stamp();
		convolution_time = SC_ZERO_TIME;
		transaction_time = SC_ZERO_TIME;
		conv0_time = SC_ZERO_TIME;
		conv1_time = SC_ZERO_TIME;
		conv2_time = SC_ZERO_TIME;

		//-------------------------------------------------------------------------------------
		// Extracting picture IF all the pictures are in the same file
		extract_pictures(picture);

		//-------------------------------------------------------------------------------------
		// Padding image
		pad_img(CONV1_PICTURE_SIZE, CONV1_NUM_CHANNELS);

		format_image(CONV1_PADDED_PICTURE_SIZE, CONV1_NUM_CHANNELS);

		if(ram.size() != CONV1_PADDED_PICTURE_SIZE * CONV1_PADDED_PICTURE_SIZE * CONV1_NUM_CHANNELS)
			cout << "ERROR IN PADDING BEFORE CONV1." << endl;

		results_file.open("../../data/conv0_input/picture_conv0_input.txt");
		for(int i = 0; i < ram.size(); i++)	results_file << ram[i] << endl;
		results_file.close();

		//-------------------------------------------------------------------------------------
		// Transfer extracted and padded original picture to Memory component

		buf=(unsigned char*)&ram[0];
		pl.set_address(PICTURE_START_ADDRESS_RAM);
		pl.set_data_length(CONV1_PADDED_PICTURE_SIZE * CONV1_PADDED_PICTURE_SIZE * CONV1_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		#ifdef PRINTS
		cout << "Padded original picture " << picture + 1 << " stored at Memory component at: " << sc_time_stamp() << endl;
		#endif

		ram.clear();

		//-------------------------------------------------------------------------------------
		#ifdef PRINTS
		cout << "Starting classification of picture " << picture + 1 << " at " << sc_time_stamp() << endl;
		#endif


		// FIRST CONV LAYER:
		// There is a total of 3*3*3*32 weights, which equals to 864 parameters
		// 864 * 2 (data in HW is 16-bit wide = 2 byts) = 1728 bytes = 1.69kB -> Fits into one BRAM cell
		// Since our HW design will require 3 weights at once, Vivado will map that BRAM cells onto 3
		// Cells (it will copy and replicate content of an original cell 2 times)

		#ifdef PRINTS
		cout << endl;
		#endif 

		//-------------------------------------------------------------------------------------
		// START IP to load CONV1 weights

		time_start = sc_time_stamp();

		ip_command = 0b00010;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(CLK_PERIOD, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV1_WEIGHTS_NUM);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load input_image

		ip_command = 0b00011;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		//-------------------------------------------------------------------------------------
		// SEND INPUT_IMAGE - TROUGH DMA TO IP BLOCK

		// CONV0 input image is the size of 34*34*3 = 3468
		// 3468 * 2 = 6936 bytes = 6.78kB -> fits into 2 BRAM cells
		// Vivado will map it onto 2 * 3 = 6 cells

		// Output of CONV0 is size 32*32*32 = 32768
		// 32768 * 2 = 65536 bytes = 64kB -> fits into 16 BRAM cells
		// Total BRAM cells occupied by CONV0 parameters: 1(bias) + 3(weights) + 6(input_image) + 16(output_image) = 26 cells

		time_start = sc_time_stamp();

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + PICTURE_START_ADDRESS_RAM);
		pl.set_data_length(CONV1_PADDED_PICTURE_SIZE * CONV1_PADDED_PICTURE_SIZE * CONV1_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		#ifdef PRINTS
		cout << "Input image for CONV0 loaded in IP block at: " << sc_time_stamp() << endl;
		#endif

		//-------------------------------------------------------------------------------------
		// START IP to do CONV0

		time_start = sc_time_stamp();

		ip_command = 0b00100;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		// CPU awaits signal from HW that convolution is done
		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv0_time = time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to send output image to Memory component

		time_start = sc_time_stamp();

		ip_command = 0b01000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);
	
		//-------------------------------------------------------------------------------------
		//SEND OUTPUT_IMAGE - TROUGH DMA TO MEMORY
		
		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV1_PICTURE_SIZE*CONV1_PICTURE_SIZE*CONV1_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// READ DATA FROM DDR FOR MAXPOOLING

		pl.set_address(WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV1_PICTURE_SIZE*CONV1_PICTURE_SIZE*CONV1_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);
		buf = pl.get_data_ptr();
		image.clear();
		for (int i = 0; i < CONV1_PICTURE_SIZE*CONV1_PICTURE_SIZE*CONV1_NUM_FILTERS; ++i)
		{
			image.push_back(((hwdata_t*)buf)[i]);
		}
		/*
		results_file.open("../../data/conv0_output/conv0_results.txt");
		for(int i = 0; i < image.size(); i++) results_file << image[i] << endl;
		results_file.close();
		*/
		//-------------------------------------------------------------------------------------
		// DO MAXPOOL 1
		transform_1D_to_4D(image, image4D, CONV1_PICTURE_SIZE, CONV1_NUM_FILTERS);
		output.clear();
		output = maxpool[0]->forward_prop(image4D, {}); 

		transform_4D_to_1D(output, image, CONV1_PICTURE_SIZE/2, CONV1_NUM_FILTERS);

		if(ram.size() != 0) ram.clear();

		// Transforming vector<float> to vector<hwdata_t>
		for (long unsigned int i = 0; i < image.size(); ++i)
		{
			ram.push_back(image[i]);
		}

		//-------------------------------------------------------------------------------------
		// Padding image
		pad_img(CONV2_PICTURE_SIZE, CONV2_NUM_CHANNELS);

		format_image(CONV2_PADDED_PICTURE_SIZE, CONV2_NUM_CHANNELS);

		if(ram.size() != CONV2_PADDED_PICTURE_SIZE * CONV2_PADDED_PICTURE_SIZE * CONV2_NUM_CHANNELS)
			cout << "ERROR IN PADDING BEFORE CONV2." << endl;

		results_file.open("../../data/conv1_input/picture_conv1_input.txt");
		for(int i = 0; i < ram.size(); i++)	results_file << ram[i] << endl;
		results_file.close();

		//-------------------------------------------------------------------------------------
		// WRITE DATA TO DDR

		buf=(unsigned char*)&ram[0];
		pl.set_address(WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV2_PADDED_PICTURE_SIZE*CONV2_PADDED_PICTURE_SIZE*CONV2_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		#ifdef PRINTS
		cout << endl;
		#endif 

		//-------------------------------------------------------------------------------------
		// START IP to load input image for CONV1

		// CONV1 is done in a following matter:
		// Input image is size of 18*18*32 = 10368
		// For that amount of data we'll need 8 BRAM cells
		// Vivado will map it onto 24 BRAM cells

		// Input image is loaded into BRAM once in full
		// Weights will be send in 2 batches and convolution will be performed
		// In 2 parts - first 16 filters will be sent to IP and convoluted
		// Output will be stored in Memory component, then the last 16 fitlers
		// Are sent to IP and CONV1 is finished

		time_start = sc_time_stamp();

		ip_command = 0b00101;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		//-------------------------------------------------------------------------------------
		// SEND INPUT_IMAGE - TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV2_PADDED_PICTURE_SIZE*CONV2_PADDED_PICTURE_SIZE*CONV2_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
		#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		#ifdef PRINTS
		cout << "Input image for CONV1 loaded in IP block at: " << sc_time_stamp() << endl;
		#endif

		//-------------------------------------------------------------------------------------
		// START IP to load FIRST HALF of CONV1 weights

		time_start = sc_time_stamp();
		
		ip_command = 0b00110;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(CLK_PERIOD, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND FIRST HALF OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM);
		pl.set_data_length(CONV2_HALF_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do first 16 filters of CONV1 (half of CONV1)

		time_start = sc_time_stamp();

		ip_command = 0b00111;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv1_time = time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load SECOND HALF of CONV1 weights

		time_start = sc_time_stamp();
		
		ip_command = 0b00110;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(CLK_PERIOD, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND SECOND HALF OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM + CONV2_HALF_WEIGHTS);
		pl.set_data_length(CONV2_HALF_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do last 16 filters of CONV1 (second half of CONV1)

		time_start = sc_time_stamp();

		ip_command = 0b00111;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv1_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to send output image to Memory component

		time_start = sc_time_stamp();

		ip_command = 0b01000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		//-------------------------------------------------------------------------------------
		// SEND OUTPUT_IMAGE - TROUGH DMA TO MEMORY

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + PICTURE_START_ADDRESS_RAM);
		pl.set_data_length(CONV2_PICTURE_SIZE*CONV2_PICTURE_SIZE*CONV2_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);
		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// READ DATA FROM DDR FOR MAXPOOLING

		pl.set_address(WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV2_PICTURE_SIZE*CONV2_PICTURE_SIZE*CONV2_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);
		buf = pl.get_data_ptr();
		image.clear();
		for (int i = 0; i < CONV2_PICTURE_SIZE*CONV2_PICTURE_SIZE*CONV2_NUM_FILTERS; ++i)
		{
			image.push_back(((hwdata_t*)buf)[i]);
		}
		/*
		results_file.open("../../data/conv1_output/conv1_results.txt");
		for(int i = 0; i < image.size(); i++) results_file << image[i] << endl;
		results_file.close();
		*/
		//-------------------------------------------------------------------------------------
		// DO MAXPOOL 2

		transform_1D_to_4D(image,image4D,CONV2_PICTURE_SIZE,CONV2_NUM_FILTERS);
		output.clear();
		output=maxpool[1]->forward_prop(image4D, {});

		transform_4D_to_1D(output,image,CONV2_PICTURE_SIZE/2,CONV2_NUM_FILTERS);

		//orig_img.clear();
		if(ram.size() != 0) ram.clear();

		//transforimng vector<float> to vector<hwdata_t>
		for (long unsigned int i = 0; i < image.size(); ++i)
		{
			ram.push_back(image[i]);
		}

		//-------------------------------------------------------------------------------------
		// Padding image
		pad_img(CONV3_PICTURE_SIZE, CONV3_NUM_CHANNELS);

		format_image(CONV3_PADDED_PICTURE_SIZE, CONV3_NUM_CHANNELS);

		if(ram.size() != CONV3_PADDED_PICTURE_SIZE * CONV3_PADDED_PICTURE_SIZE * CONV3_NUM_CHANNELS)
			cout << "ERROR IN PADDING BEFORE CONV3." << endl;


		results_file.open("../../data/conv2_input/picture_conv2_input.txt");
		for(int i = 0; i < ram.size(); i++)	results_file << ram[i] << endl;
		results_file.close();

		//-------------------------------------------------------------------------------------
		// WRITE DATA TO DDR

		buf=(unsigned char*)&ram[0];
		pl.set_address(WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV3_PADDED_PICTURE_SIZE*CONV3_PADDED_PICTURE_SIZE*CONV3_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		#ifdef PRINTS
		cout << endl;
		#endif
		
		//-------------------------------------------------------------------------------------
		// START IP to load input image for CONV2

		// CONV2 is done in a following matter:
		// Input image is size of 10*10*32 = 3200
		// For that amount of data we'll need x BRAM cells
		// Vivado will map it onto x BRAM cells

		// Input image is loaded into BRAM once in full
		// Weights will be send in 4 batches and convolution will be performed
		// In 4 parts - first 16 filters will be sent to IP and convoluted
		// Output will be stored in Memory component
		// Process will repeat 3 more times until CONV2 is finished

		time_start = sc_time_stamp();

		ip_command = 0b01001;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		//-------------------------------------------------------------------------------------
		// SEND INPUT_IMAGE - TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + PICTURE_START_ADDRESS_RAM);
		pl.set_data_length(CONV3_PADDED_PICTURE_SIZE*CONV3_PADDED_PICTURE_SIZE*CONV3_NUM_CHANNELS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif  
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load 1/4 of CONV2 weights
		
		time_start = sc_time_stamp();

		ip_command = 0b01010;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(20, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND 1/4 OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM + CONV2_WEIGHTS_NUM);
		pl.set_data_length(CONV3_SPLIT_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do first 16 filters of CONV2 (1/4 of CONV2)

		time_start = sc_time_stamp();

		ip_command = 0b10000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv2_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load second batch of 1/4 of CONV2 weights

		time_start = sc_time_stamp();
		
		ip_command = 0b01010;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(20, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND 1/4 OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM + CONV2_WEIGHTS_NUM + 1 *CONV3_SPLIT_WEIGHTS);
		pl.set_data_length(CONV3_SPLIT_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do second 16 filters of CONV2 (1/4 of CONV2)

		time_start = sc_time_stamp();

		ip_command = 0b10000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv1_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load third batch of 1/4 of CONV2 weights

		time_start = sc_time_stamp();
		
		ip_command = 0b01010;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(CLK_PERIOD, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND 1/4 OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM + CONV2_WEIGHTS_NUM + 2 *CONV3_SPLIT_WEIGHTS);
		pl.set_data_length(CONV3_SPLIT_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do third 16 filters of CONV2 (1/4 of CONV2)

		time_start = sc_time_stamp();

		ip_command = 0b10000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv2_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to load fourth (last) batch of 1/4 of CONV2 weights

		time_start = sc_time_stamp();
		
		ip_command = 0b01010;
		buf=(unsigned char*)&ip_command;

		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		#ifdef QUANTUM
		qk.inc(sc_time(CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#else
		offset += sc_time(CLK_PERIOD, SC_NS);
		#endif

		//-------------------------------------------------------------------------------------
		// SEND 1/4 OF WEIGHTS- TROUGH DMA TO IP BLOCK

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + BIAS_NUM_OF_PARAMETARS + CONV1_WEIGHTS_NUM + CONV2_WEIGHTS_NUM + 3 *CONV3_SPLIT_WEIGHTS);
		pl.set_data_length(CONV3_SPLIT_WEIGHTS);
		pl.set_command(TLM_WRITE_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while(tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to do last 16 filters of CONV2 (1/4 of CONV2)

		time_start = sc_time_stamp();

		ip_command = 0b10000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		} while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		convolution_time += time_finish - time_start;
		conv2_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		// START IP to send output image to Memory component

		time_start = sc_time_stamp();

		ip_command = 0b01000;
		buf=(unsigned char*)&ip_command;
		pl.set_address(0x80000000);
		pl.set_data_length(1);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);

		qk.set_and_sync(offset);

		//-------------------------------------------------------------------------------------
		//SEND OUTPUT_IMAGE - TROUGH DMA TO MEMORY

		// Simulating 40 CLK period delay before transaction
		#ifdef QUANTUM
		qk.inc(sc_time(40 * CLK_PERIOD, SC_NS));
		offset = qk.get_local_time();
		qk.set_and_sync(offset);
		#endif

		pl.set_address(0x81000000 + PICTURE_START_ADDRESS_RAM);
		pl.set_data_length(CONV3_PICTURE_SIZE*CONV3_PICTURE_SIZE*CONV3_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i0->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);
		do
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			tmp_sig=sig0.read();
		}while( tmp_sig == SC_LOGIC_0);

		time_finish = sc_time_stamp();
		transaction_time += time_finish - time_start;

		//-------------------------------------------------------------------------------------
		//READ DATA FROM DDR FOR MAXPOOLING

		pl.set_address(WEIGHTS_NUM_OF_PARAMETARS + BIAS_NUM_OF_PARAMETARS);
		pl.set_data_length(CONV3_PICTURE_SIZE*CONV3_PICTURE_SIZE*CONV3_NUM_FILTERS);
		pl.set_command(TLM_READ_COMMAND);
		s_cp_i1->b_transport(pl, offset);
		assert(pl.get_response_status() == TLM_OK_RESPONSE);
		qk.set_and_sync(offset);
		buf = pl.get_data_ptr();
		image.clear();
		for (int i = 0; i < CONV3_PICTURE_SIZE*CONV3_PICTURE_SIZE*CONV3_NUM_FILTERS; ++i)
		{
			image.push_back(((hwdata_t*)buf)[i]);
		}
		/*
		results_file.open("../../data/conv2_output/conv2_results.txt");
		for(int i = 0; i < image.size(); i++) results_file << image[i] << endl;
		results_file.close();
		*/
		//-------------------------------------------------------------------------------------
		// DO MAXPOOL 3
		transform_1D_to_4D(image,image4D,CONV3_PICTURE_SIZE,CONV3_NUM_FILTERS);
		output.clear();

		output = maxpool[2]->forward_prop(image4D, {});

		//-------------------------------------------------------------------------------------
		// FLATTEN LAYER
		flatten(output,dense1_input,CONV3_PICTURE_SIZE/2,CONV3_NUM_FILTERS);

		//-------------------------------------------------------------------------------------
		// DENSE LAYERS
		dense1_output=dense_layer[0]->forward_prop(dense1_input);
		dense2_output=dense_layer[1]->forward_prop(dense1_output);

		float max_value = dense2_output[0][0];
		int calculated_label = 0;
		for (int i = 0; i < 10; ++i)
		{
			if(max_value < dense2_output[0][i]) 
			{
				max_value = dense2_output[0][i];
				calculated_label = i;
			}
			cout << dense2_output[0][i] << endl;
			//results_file << dense2_output[0][i] << endl;
		}

		#ifdef PRINTS
		cout << "Picture " << picture + 1 << " output: " << calculated_label << endl;
		cout << "Correct output should be: " << labels[picture] << endl;
		#endif

		if(calculated_label == labels[picture]) 
		{
			cout << "---- HIT! ----" << endl;
			hit_number++;
		}
		else 
		{
			cout << "---- MISS! ----" << endl;
		}

		#ifdef PRINTS
		cout << "Picture " << picture + 1 << " classification finished at: " << sc_time_stamp() << endl;
		cout << "Time consumed while processing picture " << picture + 1 << ": " << sc_time_stamp() - start_time << endl;
		cout << "### Time consumed by convolution: " << convolution_time << " (" << convolution_time / (sc_time_stamp() - start_time) * 100 << "%)" << endl; 
		cout << "	 Of that, time consumed by CONV0: " << conv0_time << " which is " << conv0_time / convolution_time * 100 << "% of total convolution time." << endl;
		cout << "	 Of that, time consumed by CONV1: " << conv1_time << " which is " << conv1_time / convolution_time * 100 << "% of total convolution time." << endl;
		cout << "	 Of that, time consumed by CONV2: " << conv2_time << " which is " << conv2_time / convolution_time * 100 << "% of total convolution time." << endl;
		cout << "### Time consumed by transaction: " << transaction_time << " (" << transaction_time / (sc_time_stamp() - start_time) * 100 << "%)" << endl; 
		cout << endl << endl;
		#endif
	}
	
	//results_file.close();

	cout << "END OF CLASSIFICATION at: "<< sc_time_stamp() <<endl;
	cout << "Model accuracy is " << hit_number * 100.0 / NUM_OF_PICTURES << "%." << endl;
}

// Function that extracts weights and biases from files
// And stores them in a vector (1D structure) in a following way:
// CONV0 BIAS
// CONV1 BIAS
// CONV2 BIAS
// CONV0 WEIGHTS
// CONV1 WEIGHTS
// CONV2 WEIGHTS

void Cpu::extract_parameters()
{
	ifstream file_param;
	
	char *weights1 ="../../data/parametars/conv1/conv1_filters.txt"; 
	char *bias1 ="../../data/parametars/conv1/conv1_bias.txt"; 
	char *weights2 ="../../data/parametars/conv2/conv2_filters.txt"; 
	char *bias2 ="../../data/parametars/conv2/conv2_bias.txt"; 
	char *weights3 ="../../data/parametars/conv3/conv3_filters.txt"; 
	char *bias3 ="../../data/parametars/conv3/conv3_bias.txt"; 

	int lines;
	ram.clear();

	ofstream results_file;
	results_file.open("../../data/conv0_input/bias_formated.txt");	

	file_param.open(bias1);
	lines=num_of_lines(bias1);	

	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
		results_file << ram[ram.size() - 1] << endl;
	
	}
	file_param.close(); 
	//results_file.close();

	file_param.open(bias2);
	lines=num_of_lines(bias2);	
	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
		results_file << ram[ram.size() - 1] << endl;
	}
	file_param.close(); 

	file_param.open(bias3);
	lines=num_of_lines(bias3);	
	for (int i = 0; i < lines; ++i)
	{
		float value;
		file_param>>value;
		ram.push_back(value);
		results_file << ram[ram.size() - 1] << endl;
	}
	file_param.close(); 

	results_file.close();




	vector <hwdata_t> temp_ram;

	file_param.open(weights1);
	lines=num_of_lines(weights1);

	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;
		temp_ram.push_back(value);
	}
	file_param.close(); 

	results_file.open("../../data/conv0_input/weights0_formated.txt");	
	// Format weights 1
	for(int filter = 0; filter < CONV1_NUM_FILTERS; filter++)	
	{
		for(int i = 0; i < 3; i++)
		{
			for(int channel = 0; channel < CONV1_NUM_CHANNELS; channel++)	
			{
				for(int j = 0; j < 3; j++)
				{
					ram.push_back(temp_ram[j + 9*channel + i*3 + filter*3*3*CONV1_NUM_CHANNELS]);
					results_file << ram[ram.size() - 1] << endl;
				}
			}
		}
	}
	results_file.close();

	temp_ram.clear();

	file_param.open(weights2);
	lines=num_of_lines(weights2);	
	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;

		temp_ram.push_back(value);
	}
	file_param.close(); 

	// Format weights 2
	results_file.open("../../data/conv1_input/weights1_formated.txt");	
	for(int filter = 0; filter < CONV2_NUM_FILTERS; filter++)	
	{
		for(int i = 0; i < 3; i++)
		{
			for(int channel = 0; channel < CONV2_NUM_CHANNELS; channel++)	
			{
				for(int j = 0; j < 3; j++)
				{
					ram.push_back(temp_ram[j + 9*channel + i*3 + filter*3*3*CONV2_NUM_CHANNELS]);
					results_file << ram[ram.size() - 1] << endl;
				}
			}
		}
	}
	results_file.close();

	temp_ram.clear();

	file_param.open(weights3);
	lines=num_of_lines(weights3);	
	for (int i = 0; i < lines*3; ++i)
	{
		float value;
		file_param>>value;
		temp_ram.push_back(value);
	}
	file_param.close(); 

	// Format weights 3
	results_file.open("../../data/conv2_input/weights2_formated.txt");	
	for(int filter = 0; filter < CONV3_NUM_FILTERS; filter++)	
	{
		for(int i = 0; i < 3; i++)
		{
			for(int channel = 0; channel < CONV3_NUM_CHANNELS; channel++)	
			{
				for(int j = 0; j < 3; j++)
				{
					ram.push_back(temp_ram[j + 9*channel + i*3 + filter*3*3*CONV3_NUM_CHANNELS]);
					results_file << ram[ram.size() - 1] << endl;
				}
			}
		}
	}

	results_file.close();
	temp_ram.clear();

}


void Cpu::extract_pictures(int picture_number)
{
	ifstream file_param;
	
    char *pictures = image_file_name;
	int lines;
	float value;

	if(ram.size() != 0) ram.clear();

	file_param.open(pictures);
	lines = num_of_lines(pictures);	

	int start = picture_number * lines / 10 * PICTURE_SIZE;
	int finish = picture_number * lines / 10 * PICTURE_SIZE + lines / 10 * PICTURE_SIZE;

	for (int i = 0; i < lines * PICTURE_SIZE; ++i)
	{
		file_param >> value;

		if(i >= start && i < finish)
		{
			ram.push_back(value/255.0);
		}
	}

	file_param.close();
}


void Cpu::format_image(int img_size, int num_of_channels)
{
	vector <hwdata_t> temp_ram;

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
}

void Cpu::extract_labels()
{
	ifstream file_param;
	
	int lines;
	float value;

	if(labels.size() != 0) labels.clear();

	file_param.open(labels_file_name);
	lines = num_of_lines(labels_file_name);	

	for (int i = 0; i < lines; ++i)
	{
		file_param >> value;
		labels.push_back(value);
	}

	file_param.close();
}

int Cpu::num_of_lines(const char* file_name)
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


void Cpu::pad_img(int img_size, int num_of_channels)
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
}

#endif
