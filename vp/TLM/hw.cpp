#ifndef HW_C
#define HW_C
#include "hw.hpp"
#include <fstream>

Hardware::Hardware(sc_module_name name):sc_module(name)																									
{
	SC_THREAD(proc);
	SC_THREAD(conv0);
	SC_THREAD(conv1);
	SC_THREAD(conv2);

	s_hw_t0.register_b_transport(this, &Hardware::b_transport0); //get data from DMA
	command_reg=0;

	toggle = SC_LOGIC_0;
	sig_conv0 = SC_LOGIC_0;
	sig_conv1 = SC_LOGIC_0;
	sig_conv2 = SC_LOGIC_0;

	conv1_counter = 0;
	conv2_counter = 0;

	// Maximum input size (second convolution input)
	input_image.reserve(CONV2_PADDED_PICTURE_SIZE * CONV2_PADDED_PICTURE_SIZE * CONV2_NUM_CHANNELS);
	// Maximum output size (first convolution output)
	output_image.reserve(32768);
	//std::cout << "HW constructed" << std::endl;
}

void Hardware::proc()
{

	int transfer_number = 1;
	int num_data_from_fifo=0;
	sc_time offset=SC_ZERO_TIME;
	hwdata_t fifo_read;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	while(1)
	{
		while(	command_reg != 0b00001 && 
				command_reg != 0b00010 && 
				command_reg != 0b00011 && 
				command_reg != 0b00100 && 
				command_reg != 0b00101 && 
				command_reg != 0b00110 && 
				command_reg != 0b00111 && 
				command_reg != 0b01000 && 
				command_reg != 0b01001 && 
				command_reg != 0b01010 && 
				command_reg != 0b10000 )
		{
			#ifdef QUANTUM
		        qk.inc(sc_time(CLK_PERIOD, SC_NS));
		        offset = qk.get_local_time();
		        qk.set_and_sync(offset);
		        #else
		        offset += sc_time(CLK_PERIOD, SC_NS);
		        #endif
		}


		switch(command_reg)
		{
			case 0b00001: // load bias
				bias.clear();
				toggle = SC_LOGIC_1;
				p_out->write(toggle);
				for (int i = 0; i < BIAS_NUM_OF_PARAMETARS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}

					bias.push_back(fifo_read);
					
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);

			break;

			case 0b00010: // load CONV1(CONV0) weights
				conv1_counter = 0;
				conv2_counter = 0;
				weigts.clear();
				toggle = SC_LOGIC_1;
				p_out->write(toggle);
				for (int i = 0; i < CONV1_WEIGHTS_NUM; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}

					weigts.push_back(fifo_read);
					
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);

			break;

			case 0b00011: // load CONV0 input_picture
				input_image.clear();
				toggle = SC_LOGIC_1;
				p_out->write(toggle);
				
				for (int i = 0; i < CONV1_PADDED_PICTURE_SIZE * CONV1_PADDED_PICTURE_SIZE * CONV1_NUM_CHANNELS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}
					input_image.push_back(fifo_read);
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

			break;

			case 0b00100: // do CONV0

				sig_conv0 = SC_LOGIC_1;
				toggle = SC_LOGIC_0;
				p_out->write(toggle);

				while(sig_conv0 == SC_LOGIC_1)
				{
					#ifdef QUANTUM
					qk.inc(sc_time(CLK_PERIOD, SC_NS));
					offset = qk.get_local_time();
					qk.set_and_sync(offset);
					#endif
				}

				// Conv0 is finished - signal to the CPU that is waiting
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

			break;
			
			case 0b00101: // load CONV1 input_picture
				input_image.clear();
				
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

				for (int i = 0; i < CONV2_PADDED_PICTURE_SIZE * CONV2_PADDED_PICTURE_SIZE * CONV2_NUM_CHANNELS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}
					input_image.push_back(fifo_read);
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

			break;

			case 0b00110: // load HALF of CONV2(CONV1) weights

				weigts.clear();
				toggle = SC_LOGIC_1;
				p_out->write(toggle);
				for (int i = 0; i < CONV2_HALF_WEIGHTS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}

					weigts.push_back(fifo_read);
					
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);

			break;

			case 0b00111: // do CONV1 with HALF WEIGHTS

				sig_conv1 = SC_LOGIC_1;

				toggle = SC_LOGIC_0;
				p_out->write(toggle);

				while(sig_conv1 == SC_LOGIC_1)
				{
					#ifdef QUANTUM
					qk.inc(sc_time(CLK_PERIOD, SC_NS));
					offset = qk.get_local_time();
					qk.set_and_sync(offset);
					#endif
				}

				toggle = SC_LOGIC_1;
				p_out->write(toggle);

			break;

			case 0b01000: // send output image to Memory component

				toggle = SC_LOGIC_0;
				p_out->write(toggle);

				for (unsigned int j = 0; j < output_image.size(); ++j)
				{
					#ifdef QUANTUM
					qk.inc(sc_time(CLK_PERIOD, SC_NS));
					offset = qk.get_local_time();
					qk.set_and_sync(offset);
					#else
					offset += sc_time(CLK_PERIOD, SC_NS);
					#endif

					while(!p_fifo_out->nb_write(output_image[j]))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}
					toggle = SC_LOGIC_0;
					p_out->write(toggle);
				}
				// WAIT 10ns MORE TO DMA GET LAST NUMBER, AFTER THAT SIGNAL TO PROCESSOR THAT TRANSFER IS FINISHED
				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(CLK_PERIOD, SC_NS);
				#endif

				output_image.clear();

				toggle = SC_LOGIC_1;
				p_out->write(toggle);
			break;

			case 0b01001: // load CONV2 input_picture
				input_image.clear();
				
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

				for (int i = 0; i < CONV3_PADDED_PICTURE_SIZE * CONV3_PADDED_PICTURE_SIZE * CONV3_NUM_CHANNELS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}
					input_image.push_back(fifo_read);
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				toggle = SC_LOGIC_1;
				p_out->write(toggle);

			break;

			case 0b01010: // load 1/4 of CONV2 weights
				weigts.clear();
				toggle = SC_LOGIC_1;
				p_out->write(toggle);
				for (int i = 0; i < CONV3_SPLIT_WEIGHTS; ++i)
				{
					while(!p_fifo_in->nb_read(fifo_read))
					{
						#ifdef QUANTUM
						qk.inc(sc_time(CLK_PERIOD, SC_NS));
						offset = qk.get_local_time();
						qk.set_and_sync(offset);
						#else
						offset += sc_time(CLK_PERIOD, SC_NS);
						#endif
					}

					weigts.push_back(fifo_read);
					
					toggle =  SC_LOGIC_0; 
					p_out->write(toggle);
				}
				
				toggle =  SC_LOGIC_1; 
				p_out->write(toggle);

			break;

			case 0b10000: // do CONV2 with 1/4 of weights

				sig_conv2 = SC_LOGIC_1;

				toggle = SC_LOGIC_0;
				p_out->write(toggle);

				while(sig_conv2 == SC_LOGIC_1)
				{
					#ifdef QUANTUM
					qk.inc(sc_time(CLK_PERIOD, SC_NS));
					offset = qk.get_local_time();
					qk.set_and_sync(offset);
					#endif
				}

				toggle = SC_LOGIC_1;
				p_out->write(toggle);
			break;
			
			default:
				cout<<" HW: Nothing to be done."<<endl;
			break;
		}
		
		command_reg = 0b00000;
	}

}

void Hardware::b_transport0(pl_t& pl, sc_time& offset)
{
	tlm_command cmd    = pl.get_command();
	uint64 adr         = pl.get_address();
	const unsigned char *buf = pl.get_data_ptr();
	unsigned int len   = pl.get_data_length();
	switch(cmd)
	{
		case TLM_WRITE_COMMAND:
			command_reg = int(*buf);
			pl.set_response_status(TLM_OK_RESPONSE);
			break;
		case TLM_READ_COMMAND:
			break;
		default:
			pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}
	offset += sc_time(CLK_PERIOD, SC_NS);

}

void Hardware::conv0()
{
	vector<hwdata_t> line_buffer0;
	vector<hwdata_t> line_buffer1;
	vector<hwdata_t> line_buffer2;

	vector<hwdata_t> weights_buffer0;
	vector<hwdata_t> weights_buffer1;
	vector<hwdata_t> weights_buffer2;
	vector<hwdata_t> weights_buffer3;
	vector<hwdata_t> weights_buffer4;
	vector<hwdata_t> weights_buffer5;
	vector<hwdata_t> weights_buffer6;
	vector<hwdata_t> weights_buffer7;
	vector<hwdata_t> weights_buffer8;

	vector<hwdata_t> MAC;
	hwdata_t conv_sum;

	ofstream results_file;
	

	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	
	while(1)
	{
		while(sig_conv0 == SC_LOGIC_0)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
			
		}

		#ifdef PRINTS
		cout << "Conv0 start at: " << sc_time_stamp() << endl;
		#endif
		results_file.open("../../data/conv0_output/conv0_results.txt");

		int start_address_weights = 0; 
		int start_address_bias = 0;
		int filter_size = 3;
		int num_of_channels = 3;
		int num_of_filters = 32;
		int img_size = 34;

		// ----

		int counter_columns = 0;
		int counter_rows = 0;
		int counter_channel = 0;
		int flag_end_of_row = 0;

		int address_read0 = 0;
		int address_read1 = 0;
		int address_read2 = 0;
		int address_weights_read0 = 0;
		int address_weights_read1 = 0;
		int address_weights_read2 = 0;

		output_image.clear();
		
		for (int filter = 0; filter < num_of_filters; ++filter)
		{		
			cout << "Convoluting filter " << filter << "..." << endl;
			counter_rows = 0;
			counter_columns = 0;
			counter_channel = 0;
			flag_end_of_row = 0;

			// Resetting pixels that have been read from BRAM input image
			// For each new filter, so the counter always starts at 0
			//pixels_read = 0;
			address_read0 = 0;
			address_read1 = 1;
			address_read2 = 2;

			// Determing the start address for weights for each filter
			// Weights are formated in 3x3 slices, x3 for each channel, and every filter has it own
			// Set of 3x3x3 weights, hence x'filter'.
			//pos_weights = filter * 3*3*3;

			// Calculating starting addresses only once at the start of each filter
			address_weights_read0 = filter * 27;
			address_weights_read1 = filter * 27 + 1;
			address_weights_read2 = filter * 27 + 2;

			// Cleaning weights buffers at the start of each new filter
			weights_buffer0.clear();
			weights_buffer1.clear();
			weights_buffer2.clear();
			weights_buffer3.clear();
			weights_buffer4.clear();
			weights_buffer5.clear();
			weights_buffer6.clear();
			weights_buffer7.clear();
			weights_buffer8.clear();

			// Filling weights buffers
			for(int i = 0; i < 3; i ++)
			{
				weights_buffer0.emplace(weights_buffer0.begin(), weigts[address_weights_read0]);
				weights_buffer1.emplace(weights_buffer1.begin(), weigts[address_weights_read1]);
				weights_buffer2.emplace(weights_buffer2.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 3; i ++)
			{
				weights_buffer3.emplace(weights_buffer3.begin(), weigts[address_weights_read0]);
				weights_buffer4.emplace(weights_buffer4.begin(), weigts[address_weights_read1]);
				weights_buffer5.emplace(weights_buffer5.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 3; i ++)
			{
				weights_buffer6.emplace(weights_buffer6.begin(), weigts[address_weights_read0]);
				weights_buffer7.emplace(weights_buffer7.begin(), weigts[address_weights_read1]);
				weights_buffer8.emplace(weights_buffer8.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}


			while(counter_rows < img_size - 2)
			{
				// 9 CLK periods wasted while the buffer fills up with data
				// First reading: 0 34 68
				// Then reading: 1156 1190 1224	and so on

				// If a new row of convolution is starting
				if(counter_columns == 0 && counter_channel == 0)
				{
					
					// Cleaning line buffers for new data at the start of each new row
					line_buffer0.clear();
					line_buffer1.clear();
					line_buffer2.clear();
					
					if(counter_rows == 0)
					{
						for(int i = 0; i < 9; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 3;
			
							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 1)
					{
						address_read0 = 1;
						address_read1 = 2;
						address_read2 = 306;
						for(int i = 0; i < 9; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 2)
					{
						address_read0 = 2;
						address_read1 = 306;
						address_read2 = 408;
						for(int i = 0; i < 9; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 1;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else
					{
						//address_read2 -= 4;   next
						address_read0 = address_read2 - 204;
						address_read1 = address_read2 - 102;
						for(int i = 0; i < 9; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
							
							address_read0 = address_read2 - 203;
							address_read1 = address_read2 - 101;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}
					}
				}

				// If an output pixel has been created, or at start of convolution, 
				// Empty MAC modules for a new pixel (3x3 img slice)
				if(counter_channel == 0)
				{
					MAC.clear();
					for(int i = 0; i < 9; i++) MAC.push_back(0);

					// No time wasted since this would be done by reset=1 while for example buffer is filled up
				}

				// Image slice is buffered into a buffer and it is ready for convolution
				// From this point on, one image slice (3x3 piece) convolution is done in each CLK period

				// 9 multiplications in parallel

				MAC[0] += line_buffer0[8] * weights_buffer0[2];
				MAC[1] += line_buffer0[5] * weights_buffer1[2];
				MAC[2] += line_buffer0[2] * weights_buffer2[2];

				MAC[3] += line_buffer1[8] * weights_buffer3[2];
				MAC[4] += line_buffer1[5] * weights_buffer4[2];
				MAC[5] += line_buffer1[2] * weights_buffer5[2];

				MAC[6] += line_buffer2[8] * weights_buffer6[2];
				MAC[7] += line_buffer2[5] * weights_buffer7[2];
				MAC[8] += line_buffer2[2] * weights_buffer8[2];

				if(flag_end_of_row)
				{
					line_buffer0.emplace(line_buffer0.begin(), 0);
					line_buffer1.emplace(line_buffer1.begin(), 0);
					line_buffer2.emplace(line_buffer2.begin(), 0);			
				}
				// When convoluting pixels starting in the first row of each channel, it is neccessary to
				// Fill line buffers with 3 new pixels since those pixels have not been read yet
				else if(counter_rows == 0)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 3;
				}
				else if(counter_rows == 1)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 1;
				}
				else if(counter_rows == 2)
				{	
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 1;
					address_read2 = address_read2 + 1;
				}
				else
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read2 - 203;
					address_read1 = address_read2 - 101;
					address_read2 = address_read2 + 1;
				}

				
				weights_buffer0.emplace(weights_buffer0.begin(), weights_buffer0[2]);
				weights_buffer1.emplace(weights_buffer1.begin(), weights_buffer1[2]);
				weights_buffer2.emplace(weights_buffer2.begin(), weights_buffer2[2]);
				weights_buffer3.emplace(weights_buffer3.begin(), weights_buffer3[2]);
				weights_buffer4.emplace(weights_buffer4.begin(), weights_buffer4[2]);
				weights_buffer5.emplace(weights_buffer5.begin(), weights_buffer5[2]);
				weights_buffer6.emplace(weights_buffer6.begin(), weights_buffer6[2]);
				weights_buffer7.emplace(weights_buffer7.begin(), weights_buffer7[2]);
				weights_buffer8.emplace(weights_buffer8.begin(), weights_buffer8[2]);

				// CONTROL PART:

				// Counting channels done, when =3 one output pixels has been created
				counter_channel++;

				// When convolution has been done 3 times (number of input channels)
				// One output pixel is produced
				if(counter_channel == num_of_channels)
				{
					conv_sum = MAC[0] + MAC[1] + MAC[2] + MAC[3] + MAC[4] + MAC[5] + MAC[6] + MAC[7] + MAC[8];
				
					if(conv_sum + bias[start_address_bias + filter] > 0)
						output_image.push_back(conv_sum + bias[start_address_bias + filter]);
					else	
						output_image.push_back(0);

					results_file << output_image[output_image.size() - 1] << endl;
					// Counting channels done, when =3 one output pixels has been created
					counter_channel = 0;

					// Resetting weights reading position back to channel 0 for this filter
					//pos_weights = filter * 3*3*3;

					if(flag_end_of_row) 
					{
						counter_rows++;
						counter_columns = 0;
						flag_end_of_row = 0;
					}
					else
					{
						// Moving onto a new starting column
						counter_columns++;
					}

					// Resetting counter for weights index, each time one full-depth image slice is convoluted
					// And an output pixel is created, algorithm takes new image 3x3 slice from the first channel
					// So it needs to read the same 3x3 weights slice again
					// pos_weights = 0;

					// If all slices (32 of them) are done for each input channel
					// A row has finished and it's time to switch to other buffer
					if(counter_columns == img_size - 3)
					{
						//counter_rows++;
						//counter_columns = 0;
						flag_end_of_row = 1;

						// When a row has finished(all 3x3x34x3 pixels been read from BRAM), algorithm needs
						// 3 CLK periods to finish convoluting data that is already in line buffers, before
						// It can continue onto the new row (final 3 image 3x3 slices)
						
					}
				}

				// TIME 1 CLK PERIOD
				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#endif	
			}

		}

		qk.set_and_sync(offset);
		#ifdef PRINTS
		cout << "Conv0 finished at: " << sc_time_stamp() << endl;
		#endif

		sig_conv0 = SC_LOGIC_0;
		results_file.close();
	}
}

void Hardware::conv1()
{
	vector<hwdata_t> line_buffer0;
	vector<hwdata_t> line_buffer1;
	vector<hwdata_t> line_buffer2;

	vector<hwdata_t> weights_buffer0;
	vector<hwdata_t> weights_buffer1;
	vector<hwdata_t> weights_buffer2;
	vector<hwdata_t> weights_buffer3;
	vector<hwdata_t> weights_buffer4;
	vector<hwdata_t> weights_buffer5;
	vector<hwdata_t> weights_buffer6;
	vector<hwdata_t> weights_buffer7;
	vector<hwdata_t> weights_buffer8;

	vector<hwdata_t> MAC;
	hwdata_t conv_sum;

	ofstream results_file;
	

	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif

	while(1)
	{
		while(sig_conv1 == SC_LOGIC_0)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
		}
		
		#ifdef PRINTS
		cout << "Conv1 - part" << conv1_counter+1 << " start at: " << sc_time_stamp() << endl;
		#endif
		results_file.open("../../data/conv1_output/conv1_results.txt", ios::app);

		int start_address_weights = 0;
		int start_address_bias = CONV1_NUM_BIAS;
		int filter_size = 3;
		int num_of_channels = 32;
		int num_of_filters = 16;
		int img_size = 18;

		// ----


		int counter_columns = 0;
		int counter_rows = 0;
		int counter_channel = 0;
		int flag_end_of_row = 0;

		int address_read0 = 0;
		int address_read1 = 0;
		int address_read2 = 0;
		int address_weights_read0 = 0;
		int address_weights_read1 = 0;
		int address_weights_read2 = 0;
		
		for (int filter = 0; filter < num_of_filters; ++filter)
		{		
			cout << "Convoluting filter " << conv1_counter*16 + filter << "..." << endl;

			counter_rows = 0;
			counter_columns = 0;
			counter_channel = 0;
			flag_end_of_row = 0;

			// Resetting pixels that have been read from BRAM input image
			// For each new filter, so the counter always starts at 0
			//pixels_read = 0;
			address_read0 = 0;
			address_read1 = 1;
			address_read2 = 2;

			//pos_weights = filter*3*3*32;
			address_weights_read0 = filter * 288;
			address_weights_read1 = filter * 288 + 1;
			address_weights_read2 = filter * 288 + 2;

			// Cleaning weights buffers at the start of each new filter
			weights_buffer0.clear();
			weights_buffer1.clear();
			weights_buffer2.clear();
			weights_buffer3.clear();
			weights_buffer4.clear();
			weights_buffer5.clear();
			weights_buffer6.clear();
			weights_buffer7.clear();
			weights_buffer8.clear();

			// Filling weights buffers
			for(int i = 0; i < 32; i ++)
			{
				weights_buffer0.emplace(weights_buffer0.begin(), weigts[address_weights_read0]);
				weights_buffer1.emplace(weights_buffer1.begin(), weigts[address_weights_read1]);
				weights_buffer2.emplace(weights_buffer2.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 32; i ++)
			{
				weights_buffer3.emplace(weights_buffer3.begin(), weigts[address_weights_read0]);
				weights_buffer4.emplace(weights_buffer4.begin(), weigts[address_weights_read1]);
				weights_buffer5.emplace(weights_buffer5.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 32; i ++)
			{
				weights_buffer6.emplace(weights_buffer6.begin(), weigts[address_weights_read0]);
				weights_buffer7.emplace(weights_buffer7.begin(), weigts[address_weights_read1]);
				weights_buffer8.emplace(weights_buffer8.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			while(counter_rows < img_size - 2)
			{
				// If a new row of convolution is starting
				if(counter_columns == 0 && counter_channel == 0)
				{
					// Cleaning line buffers for new data at the start of each new row
					line_buffer0.clear();
					line_buffer1.clear();
					line_buffer2.clear();
					
					if(counter_rows == 0)
					{
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 3;
			
							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 1)
					{
						address_read0 = 1;
						address_read1 = 2;
						// 18*3*32
						address_read2 = 1728;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 2)
					{
						address_read0 = 2;
						address_read1 = 1728;
						// 18*4*32
						address_read2 = 2304;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 1;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else
					{
						// Going back 2 rows: 2*18*32
						address_read0 = address_read2 - 1152;
						address_read1 = address_read2 - 576;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
							
							address_read0 = address_read2 - 1151;
							address_read1 = address_read2 - 575;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}
					}
				}	

				// If an output pixel has been created, or at start of convolution, 
				// Empty MAC modules for a new pixel (3x3 img slice)
				if(counter_channel == 0)
				{
					MAC.clear();
					for(int i = 0; i < 9; i++) MAC.push_back(0);
				}

				// Image slice is buffered into a buffer and it is ready for convolution
				// From this point on, one image slice (3x3 piece) convolution is done in each CLK period

				// 9 multiplications in parallel

				MAC[0] += line_buffer0[95] * weights_buffer0[31];
				MAC[1] += line_buffer0[63] * weights_buffer1[31];
				MAC[2] += line_buffer0[31] * weights_buffer2[31];

				MAC[3] += line_buffer1[95] * weights_buffer3[31];
				MAC[4] += line_buffer1[63] * weights_buffer4[31];
				MAC[5] += line_buffer1[31] * weights_buffer5[31];

				MAC[6] += line_buffer2[95] * weights_buffer6[31];
				MAC[7] += line_buffer2[63] * weights_buffer7[31];
				MAC[8] += line_buffer2[31] * weights_buffer8[31];

				if(flag_end_of_row)
				{
					line_buffer0.emplace(line_buffer0.begin(), 0);
					line_buffer1.emplace(line_buffer1.begin(), 0);
					line_buffer2.emplace(line_buffer2.begin(), 0);			
				}
				// When convoluting pixels starting in the first row of each channel, it is neccessary to
				// Fill line buffers with 3 new pixels since those pixels have not been read yet
				else if(counter_rows == 0)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 3;
				}
				// 
				else if(counter_rows == 1)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 1;
				}
				else if(counter_rows == 2)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 1;
					address_read2 = address_read2 + 1;
				}
				else
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read2 - 1151;
					address_read1 = address_read2 - 575;
					address_read2 = address_read2 + 1;
				}

				weights_buffer0.emplace(weights_buffer0.begin(), weights_buffer0[31]);
				weights_buffer1.emplace(weights_buffer1.begin(), weights_buffer1[31]);
				weights_buffer2.emplace(weights_buffer2.begin(), weights_buffer2[31]);
				weights_buffer3.emplace(weights_buffer3.begin(), weights_buffer3[31]);
				weights_buffer4.emplace(weights_buffer4.begin(), weights_buffer4[31]);
				weights_buffer5.emplace(weights_buffer5.begin(), weights_buffer5[31]);
				weights_buffer6.emplace(weights_buffer6.begin(), weights_buffer6[31]);
				weights_buffer7.emplace(weights_buffer7.begin(), weights_buffer7[31]);
				weights_buffer8.emplace(weights_buffer8.begin(), weights_buffer8[31]);

				// CONTROL PART:

				// Counting channels done, when =32 one output pixels has been created
				counter_channel++;

				// When convolution has been done 32 times (number of input channels)
				// One output pixel is produced
				if(counter_channel == num_of_channels)
				{
					conv_sum = MAC[0] + MAC[1] + MAC[2] + MAC[3] + MAC[4] + MAC[5] + MAC[6] + MAC[7] + MAC[8];
				
					if(conv_sum + bias[start_address_bias + conv1_counter*16 + filter] > 0)
						output_image.push_back(conv_sum + bias[start_address_bias + conv1_counter*16 + filter]);
					else	
						output_image.push_back(0);

					results_file << output_image[output_image.size() - 1] << endl;

					// Counting channels done, when =3 one output pixels has been created
					counter_channel = 0;

					// Resetting weights reading position back to channel 0 for this filter
					//pos_weights = filter*3*3*32;

					if(flag_end_of_row) 
					{
						counter_rows++;
						counter_columns = 0;
						flag_end_of_row = 0;
					}
					else
					{
						// Moving onto a new starting column
						counter_columns++;
					}

					// Resetting counter for weights index, each time one full-depth image slice is convoluted
					// And an output pixel is created, algorithm takes new image 3x3 slice from the first channel
					// So it needs to read the same 3x3 weights slice again
					// pos_weights = 0;

					// If all slices (32 of them) are done for each input channel
					// A row has finished and it's time to switch to other buffer
					if(counter_columns == img_size - 3)
					{
						flag_end_of_row = 1;				
					}
				}

				// TIME 1 CLK PERIOD
				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#endif	
			}

		}

		results_file.close();

		qk.set_and_sync(offset);
		#ifdef PRINTS
		cout << "Conv1 - part" << conv1_counter+1 << " finished at: " << sc_time_stamp() << endl;
		#endif

		conv1_counter++;
		sig_conv1 = SC_LOGIC_0;
	}
}


void Hardware::conv2()
{
	vector<hwdata_t> line_buffer0;
	vector<hwdata_t> line_buffer1;
	vector<hwdata_t> line_buffer2;

	vector<hwdata_t> weights_buffer0;
	vector<hwdata_t> weights_buffer1;
	vector<hwdata_t> weights_buffer2;
	vector<hwdata_t> weights_buffer3;
	vector<hwdata_t> weights_buffer4;
	vector<hwdata_t> weights_buffer5;
	vector<hwdata_t> weights_buffer6;
	vector<hwdata_t> weights_buffer7;
	vector<hwdata_t> weights_buffer8;

	vector<hwdata_t> MAC;
	hwdata_t conv_sum;

	ofstream results_file;

	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	
	while(1)
	{
		while(sig_conv2 == SC_LOGIC_0)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(CLK_PERIOD, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#endif
		}
		
		#ifdef PRINTS
		cout << "Conv2 - part" << conv2_counter+1 << " start at: " << sc_time_stamp() << endl;
		#endif
		results_file.open("../../data/conv2_output/conv2_results.txt", ios::app);	
		int img_size = 10;
		int start_address_weights = 0;
		int start_address_bias = CONV1_NUM_BIAS + CONV2_NUM_BIAS;
		int filter_size = 3;
		int num_of_channels = 32;
		int num_of_filters = 64 / 4;

		
		// ----

		int counter_columns = 0;
		int counter_rows = 0;
		int counter_channel = 0;
		int flag_end_of_row = 0;

		int address_read0 = 0;
		int address_read1 = 0;
		int address_read2 = 0;
		int address_weights_read0 = 0;
		int address_weights_read1 = 0;
		int address_weights_read2 = 0;

		
		for (int filter = 0; filter < num_of_filters; ++filter)
		{		
			cout << "Convoluting filter " << conv2_counter*16 + filter << "..." << endl;

			counter_rows = 0;
			counter_columns = 0;
			counter_channel = 0;
			flag_end_of_row = 0;

			// Resetting pixels that have been read from BRAM input image
			// For each new filter, so the counter always starts at 0
			//pixels_read = 0;
			address_read0 = 0;
			address_read1 = 1;
			address_read2 = 2;

			//pos_weights = filter*3*3*32;
			address_weights_read0 = filter * 288;
			address_weights_read1 = filter * 288 + 1;
			address_weights_read2 = filter * 288 + 2;

			// Cleaning weights buffers at the start of each new filter
			weights_buffer0.clear();
			weights_buffer1.clear();
			weights_buffer2.clear();
			weights_buffer3.clear();
			weights_buffer4.clear();
			weights_buffer5.clear();
			weights_buffer6.clear();
			weights_buffer7.clear();
			weights_buffer8.clear();

			// Filling weights buffers
			for(int i = 0; i < 32; i ++)
			{
				weights_buffer0.emplace(weights_buffer0.begin(), weigts[address_weights_read0]);
				weights_buffer1.emplace(weights_buffer1.begin(), weigts[address_weights_read1]);
				weights_buffer2.emplace(weights_buffer2.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 32; i ++)
			{
				weights_buffer3.emplace(weights_buffer3.begin(), weigts[address_weights_read0]);
				weights_buffer4.emplace(weights_buffer4.begin(), weigts[address_weights_read1]);
				weights_buffer5.emplace(weights_buffer5.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			for(int i = 0; i < 32; i ++)
			{
				weights_buffer6.emplace(weights_buffer6.begin(), weigts[address_weights_read0]);
				weights_buffer7.emplace(weights_buffer7.begin(), weigts[address_weights_read1]);
				weights_buffer8.emplace(weights_buffer8.begin(), weigts[address_weights_read2]);

				address_weights_read0 = address_weights_read0 + 3;
				address_weights_read1 = address_weights_read1 + 3;
				address_weights_read2 = address_weights_read2 + 3;
			}

			while(counter_rows < img_size - 2)
			{
				// If a new row of convolution is starting
				if(counter_columns == 0 && counter_channel == 0)
				{

					// Cleaning line buffers for new data at the start of each new row
					line_buffer0.clear();
					line_buffer1.clear();
					line_buffer2.clear();
					
					if(counter_rows == 0)
					{
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 3;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 1)
					{
						address_read0 = 1;
						address_read1 = 2;
						// 10*3*32
						address_read2 = 960;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 3;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else if(counter_rows == 2)
					{
						address_read0 = 2;
						address_read1 = 960;
						// 10*4*32
						address_read2 = 1280;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

							address_read0 = address_read0 + 3;
							address_read1 = address_read1 + 1;
							address_read2 = address_read2 + 1;

							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}	
					}
					else
					{
						// Going back 2 rows: 2*10*32
						address_read0 = address_read2 - 640;
						address_read1 = address_read2 - 320;
						for(int i = 0; i < 32*3; i++)
						{
							line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
							line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
							line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
							
							address_read0 = address_read2 - 639;
							address_read1 = address_read2 - 319;
							address_read2 = address_read2 + 1;
							// TIME 1 CLK PERIOD
							#ifdef QUANTUM
							qk.inc(sc_time(CLK_PERIOD, SC_NS));
							offset = qk.get_local_time();
							qk.set_and_sync(offset);
							#endif
						}
					}
				}	

				// If an output pixel has been created, or at start of convolution, 
				// Empty MAC modules for a new pixel (3x3 img slice)
				if(counter_channel == 0)
				{
					MAC.clear();
					for(int i = 0; i < 9; i++) MAC.push_back(0);
				}

				// Image slice is buffered into a buffer and it is ready for convolution
				// From this point on, one image slice (3x3 piece) convolution is done in each CLK period

				// 9 multiplications in parallel

				MAC[0] += line_buffer0[95] * weights_buffer0[31];
				MAC[1] += line_buffer0[63] * weights_buffer1[31];
				MAC[2] += line_buffer0[31] * weights_buffer2[31];

				MAC[3] += line_buffer1[95] * weights_buffer3[31];
				MAC[4] += line_buffer1[63] * weights_buffer4[31];
				MAC[5] += line_buffer1[31] * weights_buffer5[31];

				MAC[6] += line_buffer2[95] * weights_buffer6[31];
				MAC[7] += line_buffer2[63] * weights_buffer7[31];
				MAC[8] += line_buffer2[31] * weights_buffer8[31];

				if(flag_end_of_row)
				{
					line_buffer0.emplace(line_buffer0.begin(), 0);
					line_buffer1.emplace(line_buffer1.begin(), 0);
					line_buffer2.emplace(line_buffer2.begin(), 0);			
				}
				// When convoluting pixels starting in the first row of each channel, it is neccessary to
				// Fill line buffers with 3 new pixels since those pixels have not been read yet
				else if(counter_rows == 0)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 3;
				}
				// 
				else if(counter_rows == 1)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);

					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 3;
					address_read2 = address_read2 + 1;
				}
				else if(counter_rows == 2)
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read0 + 3;
					address_read1 = address_read1 + 1;
					address_read2 = address_read2 + 1;
				}
				else
				{
					line_buffer0.emplace(line_buffer0.begin(), input_image[address_read0]);
					line_buffer1.emplace(line_buffer1.begin(), input_image[address_read1]);
					line_buffer2.emplace(line_buffer2.begin(), input_image[address_read2]);
					
					address_read0 = address_read2 - 639;
					address_read1 = address_read2 - 319;
					address_read2 = address_read2 + 1;
				}

				weights_buffer0.emplace(weights_buffer0.begin(), weights_buffer0[31]);
				weights_buffer1.emplace(weights_buffer1.begin(), weights_buffer1[31]);
				weights_buffer2.emplace(weights_buffer2.begin(), weights_buffer2[31]);
				weights_buffer3.emplace(weights_buffer3.begin(), weights_buffer3[31]);
				weights_buffer4.emplace(weights_buffer4.begin(), weights_buffer4[31]);
				weights_buffer5.emplace(weights_buffer5.begin(), weights_buffer5[31]);
				weights_buffer6.emplace(weights_buffer6.begin(), weights_buffer6[31]);
				weights_buffer7.emplace(weights_buffer7.begin(), weights_buffer7[31]);
				weights_buffer8.emplace(weights_buffer8.begin(), weights_buffer8[31]);

				// CONTROL PART:

				// Counting channels done, when =32 one output pixels has been created
				counter_channel++;

				// When convolution has been done 32 times (number of input channels)
				// One output pixel is produced
				if(counter_channel == num_of_channels)
				{
					conv_sum = MAC[0] + MAC[1] + MAC[2] + MAC[3] + MAC[4] + MAC[5] + MAC[6] + MAC[7] + MAC[8];
				
					if(conv_sum + bias[start_address_bias + conv2_counter*16 + filter] > 0)
						output_image.push_back(conv_sum + bias[start_address_bias + conv2_counter*16 + filter]);
					else	
						output_image.push_back(0);

					results_file << output_image[output_image.size() - 1] << endl;

					// Counting channels done, when =3 one output pixels has been created
					counter_channel = 0;

					// Resetting weights reading position back to channel 0 for this filter
					//pos_weights = filter*3*3*32;

					if(flag_end_of_row) 
					{
						counter_rows++;
						counter_columns = 0;
						flag_end_of_row = 0;
					}
					else
					{
						// Moving onto a new starting column
						counter_columns++;
					}

					// Resetting counter for weights index, each time one full-depth image slice is convoluted
					// And an output pixel is created, algorithm takes new image 3x3 slice from the first channel
					// So it needs to read the same 3x3 weights slice again
					// pos_weights = 0;

					// If all slices (32 of them) are done for each input channel
					// A row has finished and it's time to switch to other buffer
					if(counter_columns == img_size - 3)
					{
						flag_end_of_row = 1;				
					}
				}

				// TIME 1 CLK PERIOD
				#ifdef QUANTUM
				qk.inc(sc_time(CLK_PERIOD, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#endif	
			}

		}

		results_file.close();
		
		qk.set_and_sync(offset);
		#ifdef PRINTS
		cout << "Conv2 - part" << conv2_counter+1 << " finished at: " << sc_time_stamp() << endl;
		#endif
		
		conv2_counter++;
		sig_conv2 = SC_LOGIC_0;
	}
}


#endif