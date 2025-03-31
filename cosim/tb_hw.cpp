#include "tb_hw.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <array>
#include <algorithm>
#include <cstdio>
#include <bitset>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

/*
Casting floating point data to binary - every floating number needs to be represented as an 16 bit integer
In our system [4, 12] format is being used - 4 bits for integer part and 12 for floating part

4 MSB bits:
	MSB bit is used for sign: 0 - positive, 1 - negative
	Other 3 bits are used to represent numbers from 0 to 7
12 LSB bits:
	Can represent numbers from 0 to 99 (0.0 to 0.99)

Casting is done in a following matter:
If a floating point of value 1.0 is passed as a parameter, output should be 0 001 0000 0000 0000 = 4096
Anything less than 1.0 is scaled by a factor of 4096
Max number is 7.9999 * 4096 = 32767 = 0 111 1111 1111 1111
A negative numbers are just added a 1 on MSB by adding 32768 to the value (mask of 1 000 0000 0000 0000)
*/

uint16_t castFloatToBin(float t) {
    int sign = (t >= 0) ? 0 : 1;
    int integerPart;
    int decimalPart;
    uint16_t binaryValue;
    
    if (sign == 0)
    {
        integerPart = static_cast<int>(t);
        //cout<<"integer part : "<<std::bitset<3>(integerPart)<<endl;
        decimalPart = static_cast<int>((t - integerPart)*4096);
        //cout<<"decimal part : "<<std::bitset<12>(decimalPart)<<endl;
        binaryValue = (sign << 15) | ((integerPart & 0x7) << 12) | (decimalPart & 0xFFF);
    
    }
    else
    {
        integerPart = static_cast<int>((-1)*t);
        //cout<<"integer part : "<<std::bitset<3>(integerPart)<<endl;
        decimalPart = static_cast<int>(((-1)*t - integerPart)*4096);
        //cout<<"decimal part : "<<std::bitset<12>(decimalPart)<<endl;
        binaryValue = (sign << 15) | (((~integerPart) & 0x7) << 12) | ((~decimalPart) & 0xFFF);
	binaryValue = binaryValue + 0b000000000001;

    }

    
    return binaryValue;
}

/*
uint16_t castFloatToBin(float t) {
    int sign = (t >= 0) ? 0 : 1;
    float resolution = 0.000244140625;
    float half_of_resolution = 0.0001220703125;
    int deo;
    int integerPart;
    int decimalPart;
    uint16_t binaryValue;
    
    if (sign == 0)
    {
        deo = t/resolution;
        if(t >= deo*resolution+half_of_resolution)
            deo++;
        //cout<<"deo : "<<deo<<endl;
         binaryValue=deo;
    
    }
    else
    {
         deo = t/resolution*(-1);
         if(t <= (-1)*deo*resolution-half_of_resolution)
             deo++;
         //cout<<"deo : "<<65536-deo<<endl;

         binaryValue = 65536-deo;


    }

return binaryValue;
}
*/
float castBinToFloat(sc_lv<16> binaryValue) {

    uint16_t binaryValue_uint = binaryValue.to_uint();
    int sign = (binaryValue_uint >> 15) & 0x1;

    if (sign == 1)
    {
        binaryValue_uint = (~binaryValue_uint) + 1; // prebacujemo u pozitivno, posle cemo float pomnozitit sa -1
    }
    int integerPart = (binaryValue_uint >> 12) & 0x7;
    int decimalPart = binaryValue_uint & 0xFFF;

    float floatValue = static_cast<float>(integerPart) + (static_cast<float>(decimalPart) / 4096.0f);
    if(sign == 1)
        floatValue = floatValue *(-1);
    
    return floatValue;
}

SC_HAS_PROCESS(tb_hw);

tb_hw::tb_hw(sc_module_name name) :
	sc_module(name),
	dut("dut"),
	clk("clk", 10, SC_NS)
{
	SC_THREAD(gen_thread);
	SC_METHOD(mon_thread);
	dont_initialize();
    	// Sensitive to output signal
	//sensitive << axim_s_data;

	maxpool[0]=new MaxPoolLayer(2);
	maxpool[1]=new MaxPoolLayer(2);
	maxpool[2]=new MaxPoolLayer(2);
	dense_layer[0] = new DenseLayer(1024,512,0);
	dense_layer[1] = new DenseLayer(512,10,1);
	dense_layer[0]->load_dense_layer("../data/parametars/dense1/dense1_weights.txt","../data/parametars/dense1/dense1_bias.txt");
	dense_layer[1]->load_dense_layer("../data/parametars/dense2/dense2_weights.txt","../data/parametars/dense2/dense2_bias.txt");

	dut.interupt_done(interupt_done);

	dut.axis_s_data_in(axis_s_data_in);
	dut.axis_s_valid(axis_s_valid);
	dut.axis_s_last(axis_s_last);
   	dut.axis_s_ready(axis_s_ready);
   	dut.axis_s_tkeep(axis_s_tkeep);

	dut.axim_s_valid(axim_s_valid);
	dut.axim_s_last(axim_s_last);
	dut.axim_s_ready(axim_s_ready);
    	dut.axim_s_data(axim_s_data);
	dut.axim_s_tkeep(axim_s_tkeep);

	dut.s00_axi_aclk( clk.signal() );
	dut.s00_axi_aresetn(s00_axi_aresetn);
	dut.s00_axi_awaddr(s00_axi_awaddr);
	dut.s00_axi_awprot(s00_axi_awprot);
	dut.s00_axi_awvalid(s00_axi_awvalid);
	dut.s00_axi_awready(s00_axi_awready);
	dut.s00_axi_wdata(s00_axi_wdata);
	dut.s00_axi_wstrb(s00_axi_wstrb);
	dut.s00_axi_wvalid(s00_axi_wvalid);
	dut.s00_axi_wready(s00_axi_wready);
	dut.s00_axi_bresp(s00_axi_bresp);
	dut.s00_axi_bvalid(s00_axi_bvalid);
	dut.s00_axi_bready(s00_axi_bready);
	dut.s00_axi_araddr(s00_axi_araddr);
	dut.s00_axi_arprot(s00_axi_arprot);
	dut.s00_axi_arvalid(s00_axi_arvalid);
	dut.s00_axi_arready(s00_axi_arready);
	dut.s00_axi_rdata(s00_axi_rdata);
	dut.s00_axi_rresp(s00_axi_rresp);
	dut.s00_axi_rvalid(s00_axi_rvalid);
	dut.s00_axi_rready(s00_axi_rready);
	

	bias.clear();
	weights.clear();
	picture.clear();
}

void tb_hw::gen_thread()
{
	ostringstream ss;
	string s;
	float temp;
	int temp_int;
	vector<uint16_t> temp_weights;
	ifstream file;
	ofstream results;
	int brojac =0;

	ss << "Starting SIMULATION";
	SC_REPORT_INFO(name(), ss.str().c_str());
	ss.str("");
	ss.clear();
//-------------------------------------------------------------------------------UCITAVANJE PODATAKA---------------------------------------------------------------------------------------------		
	file.open("../data/conv0_input/bias_formated.txt");	
	//load bias in variable
	for(int i = 0; i < 128; i++)
	{
		file >> temp;
		bias.push_back(castFloatToBin(temp));
	}
	file.close();

	
	
	temp_weights.clear();
	
	wait(5, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------c
	



	axis_s_valid.write(SC_LOGIC_0);
	axis_s_last.write(SC_LOGIC_0);

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(30,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);

//-------------------------------------------------------------------------------AXI_LITE - RESET_COMMAND---------------------------------------------------------------------------------------------		
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(RESET_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(40, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	


//-------------------------------------------------------------------------------AXI_LITE - SEND_BIAS---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_BIAS_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------AXI_S_SLAVE - SEND_BIAS---------------------------------------------------------------------------------------------		
	brojac=0;
	while(brojac < 128)
	{
		if(brojac == 127)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write(sc_dt::sc_lv<16>(bias[brojac]));				
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//load weights
	weights.clear();
	file.open("../data/conv0_input/weights0_formated.txt");
	for(int i = 0; i < 864; i++)
	{
		file >> temp;
		weights.push_back(castFloatToBin(temp));
	}
	file.close();

	
//-------------------------------------------------------------------------------AXI_LITE - SEND_WEIGHTS_0---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_0_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------AXI_S_SLAVE - SEND_WEIGHTS_0---------------------------------------------------------------------------------------------		

	brojac=0;
	while(brojac < 864)
	{
		if(brojac == 863)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write(sc_dt::sc_lv<16>(weights[brojac]));			

			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}

	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	

//-------------------------------------------------------------------------------AXI_LITE - SEND_PICTURE_0---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_PICTURE_0_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	picture.clear();
	file.open("../data/conv0_input/picture_conv0_input.txt");
	for(int i = 0; i < 3468; i++)
	{
		file >> temp;
		picture.push_back(castFloatToBin(temp));
	}
	file.close();
//----------------------------------------------------------------------------AXI_S_SLAVE - SEND_PICTURE_0---------------------------------------------------------------------------------------------		


	brojac=0;
	while(brojac < 3468)
	{
		if(brojac == 3467)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(picture[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------AXI_LITE - DO_CONV0---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_0_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------AXI_LITE -  LOAD_OUTPUT_0---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(SEND_OUTPUT_FROM_CONV_0_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------AXI_S_MASTER - LOAD_OUTPUT_0---------------------------------------------------------------------------------------------		


	axim_s_ready.write(SC_LOGIC_1);
	int brojac_dol_pod = 0; 
	image.clear();
	while(interupt_done.read() == SC_LOGIC_0) 
	{
		if(axim_s_valid.read() == SC_LOGIC_1)
		{

			image.push_back(castBinToFloat(axim_s_data.read()));
		}
		
		wait(10, SC_NS);
		
	}
	axim_s_ready.write(SC_LOGIC_0);


//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	cout<< "velicina vracenih podataka nakon CONV0 : "<<image.size()<<endl;
	


	transform_1D_to_4D(image, image4D, CONV1_PICTURE_SIZE, CONV1_NUM_FILTERS);

	output.clear();
	output = maxpool[0]->forward_prop(image4D, {}); 
	transform_4D_to_1D(output, image, CONV1_PICTURE_SIZE/2, CONV1_NUM_FILTERS);
	picture.clear();
	
	for(int i = 0; i < image.size(); i++) picture.push_back(castFloatToBin(image[i]));
	
	
	pad_img(CONV2_PICTURE_SIZE, CONV2_NUM_CHANNELS);
	format_image(CONV2_PADDED_PICTURE_SIZE, CONV2_NUM_CHANNELS);
	
	cout<< "Velicina koja se salje u CONV1 : "<<picture.size()<<endl;

//-------------------------------------------------------------------------------AXI_LITE - RESET_COMMAND---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);	
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(RESET_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(40, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE -  LOAD_PICTURE_1---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_PICTURE_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_PICTURE_1---------------------------------------------------------------------------------------------		


	brojac=0;
	while(brojac < 10368)
	{
		if(brojac == 10367)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(picture[brojac]));
			// results<<int16_t (brojac)<<endl;
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



// read weights for conv1 from file
		
	temp_weights.clear();
	weights.clear();

	file.open("../data/conv1_input/weights1_formated.txt");
	for(int i = 0; i < 9216; i++)
	{
		file >> temp;
		weights.push_back(castFloatToBin(temp));
	}
	file.close();



//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	

//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_1  1/2---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_1  1/2---------------------------------------------------------------------------------------		


	brojac=0;
	while(brojac < 4608)
	{
		if(brojac == 4607)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV1  1/2---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_1  2/2---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_1  2/2--------------------------------c-------------------------------------------------------		


	brojac=4608;
	while(brojac < 9216)
	{
		if(brojac == 9215)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV1  2/2---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------AXI_LITE -  LOAD_OUTPUT_1---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(SEND_OUTPUT_FROM_CONV_1_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------AXI_S_MASTER - LOAD_OUTPUT_1---------------------------------------------------------------------------------------------		


	axim_s_ready.write(SC_LOGIC_1);
	brojac_dol_pod = 0; 
	image.clear();
	while(interupt_done.read() == SC_LOGIC_0) 
	{
		if(axim_s_valid.read() == SC_LOGIC_1)
		{

			image.push_back(castBinToFloat(axim_s_data.read()));
		}
		
		wait(10, SC_NS);
		
	}
	axim_s_ready.write(SC_LOGIC_0);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	cout<< "velicina vracenih podataka nakon CONV1 : "<<image.size()<<endl;
	
	 
	transform_1D_to_4D(image, image4D, CONV2_PICTURE_SIZE, CONV2_NUM_FILTERS);
	output.clear();
	output = maxpool[1]->forward_prop(image4D, {}); 
	transform_4D_to_1D(output, image, CONV2_PICTURE_SIZE/2, CONV2_NUM_FILTERS);
	picture.clear();
	for(int i = 0; i < image.size(); i++) picture.push_back(castFloatToBin(image[i]));

	pad_img(CONV3_PICTURE_SIZE, CONV3_NUM_CHANNELS);
	format_image(CONV3_PADDED_PICTURE_SIZE, CONV3_NUM_CHANNELS);


	cout<< "Velicina koja se salje u CONV2 : "<<picture.size()<<endl;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	

//-------------------------------------------------------------------------------AXI_LITE - RESET_COMMAND---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);	
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(RESET_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//load weights2
	temp_weights.clear();
	weights.clear();

	file.open("../data/conv2_input/weights2_formated.txt");
	for(int i = 0; i < 3*3*32*64; i++)
	{
		file >> temp;
		weights.push_back(castFloatToBin(temp));
	}
	file.close();
	
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	




//-------------------------------------------------------------------------------AXI_LITE -  LOAD_PICTURE_2---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_PICTURE_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_PICTURE_2---------------------------------------------------------------------------------------------		


	brojac=0;
	while(brojac < 3200)
	{
		if(brojac == 3199)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(picture[brojac]));
			// results<<int16_t (brojac)<<endl;
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_2  1/4---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_2  1/4--------------------------------c-------------------------------------------------------		


	brojac=0;
	while(brojac < 4608)
	{
		if(brojac == 4607)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV2  1/4---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_2  2/4---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_2  2/4--------------------------------c-------------------------------------------------------		


	brojac=4608*1;
	while(brojac < 4608*2)
	{
		if(brojac == 4608*2-1)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV2  2/4---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------








//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_2  3/4---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_2  3/4--------------------------------c-------------------------------------------------------		


	brojac=4608*2;
	while(brojac < 4608*3)
	{
		if(brojac == 4608*3-1)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV2  3/4---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------AXI_LITE -  LOAD_WEIGHTS_2  4/4---------------------------------------------------------------------------------------		
	

	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(LOAD_WEIGHTS_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	
//----------------------------------------------------------------------------AXI_S_SLAVE - LOAD_WEIGHTS_2  4/4--------------------------------c-------------------------------------------------------		


	brojac=4608*3;
	while(brojac < 4608*4)
	{
		if(brojac == 4608*4-1)
			axis_s_last.write( SC_LOGIC_1 );
		if(axis_s_ready.read() == SC_LOGIC_1)
		{
			axis_s_valid.write( SC_LOGIC_1 );
			axis_s_data_in.write( sc_dt::sc_lv<16>(weights[brojac]));
			brojac = brojac+1;			
		}	
		wait(10, SC_NS);
	}
	axis_s_valid.write( SC_LOGIC_0 );
	axis_s_last.write( SC_LOGIC_0 );
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------AXI_LITE - DO_CONV2  4/4---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(DO_CONV_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	while(interupt_done.read() == SC_LOGIC_0) 
	{
		wait(10, SC_NS);
	}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------AXI_LITE -  LOAD_OUTPUT_2---------------------------------------------------------------------------------------------		
	s00_axi_aresetn.write(SC_LOGIC_0);
	wait(20,SC_NS);
	s00_axi_aresetn.write(SC_LOGIC_1);
	s00_axi_awaddr.write(0b0000);
	s00_axi_wdata.write(SEND_OUTPUT_FROM_CONV_2_CMD);
	s00_axi_wstrb.write(0b1111);
	s00_axi_awvalid.write(SC_LOGIC_1);
	s00_axi_wvalid.write(SC_LOGIC_1);
	s00_axi_bready.write(SC_LOGIC_1);
	while(s00_axi_awready.read() == SC_LOGIC_0)
	{
		wait(10, SC_NS);
	}
	while(s00_axi_awready.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	wait(20, SC_NS);
	
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_awvalid.write(SC_LOGIC_0);
	s00_axi_wvalid.write(SC_LOGIC_0);
	s00_axi_wstrb.write(0b0000);

	while(s00_axi_bvalid.read() == SC_LOGIC_1)
	{
		wait(10, SC_NS);
	}
	s00_axi_bready.write(SC_LOGIC_0);
	wait(10, SC_NS);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------AXI_S_MASTER - LOAD_OUTPUT_2---------------------------------------------------------------------------------------------		


	axim_s_ready.write(SC_LOGIC_1);
	brojac_dol_pod = 0; 
	image.clear();
	while(interupt_done.read() == SC_LOGIC_0) 
	{
		if(axim_s_valid.read() == SC_LOGIC_1)
		{

			image.push_back(castBinToFloat(axim_s_data.read()));
		}
		
		wait(10, SC_NS);
		
	}
	axim_s_ready.write(SC_LOGIC_0);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



	transform_1D_to_4D(image, image4D, CONV3_PICTURE_SIZE, CONV3_NUM_FILTERS);
	output.clear();
	output = maxpool[2]->forward_prop(image4D, {}); 
	//transform_4D_to_1D(output, image, CONV3_PICTURE_SIZE/2, CONV3_NUM_FILTERS);
	//picture.clear();
	//for(int i = 0; i < image.size(); i++) picture.push_back((uint16_t)image[i]);

	//cout<< "Velicina koja se salje u dense1 : "<<output.size()<<endl;




	flatten(output,dense1_input,CONV3_PICTURE_SIZE/2,CONV3_NUM_FILTERS);

	dense1_output=dense_layer[0]->forward_prop(dense1_input);

	dense2_output=dense_layer[1]->forward_prop(dense1_output);

	cout << "Picture 0 results: " << endl;
	for (int i = 0; i < 10; ++i)
	{
		cout << dense2_output[0][i] << endl;
	}

	ss << "@" << sc_time_stamp();
	ss << " END SIM";
	SC_REPORT_INFO(name(), ss.str().c_str());
	
	sc_stop();
}

void tb_hw::mon_thread()
{
	
	
}

void tb_hw::pad_img(int img_size, int num_of_channels)
{
	for(int channel = 0 ; channel < num_of_channels; channel++)
    	{
		// Firstly, zeros are emplaced for the first padded row (image_size(one row) + 2 for the edges)
	        for (int i = 0; i < img_size+2; i++)
	        {
	        	picture.emplace((picture.begin() + (channel*(img_size+2)*(img_size+2)) + i), 0);
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
	        	picture.emplace((picture.begin() + pos1), 0);
	        	picture.emplace((picture.begin() + pos2), 0);
	        }

		// Finally, zeros are pushed back as we fill up the final row of the padded image (plus 2 for edges)
	        for (int i = 0; i < img_size + 2; i++)
	        {
	        	picture.emplace((picture.begin() + ((channel)*(img_size+2)*(img_size+2)) + (img_size+2)*(img_size+1) + i), 0);
	        }
    	}
}

void tb_hw::format_image(int img_size, int num_of_channels)
{
	vector<uint16_t> temp_ram;

	temp_ram.clear();
	
	for(int i = 0; i < img_size; i++)
	{
		for(int j = 0; j < num_of_channels; j++)
		{
			for(int k = 0; k < 3; k++)
			{
				temp_ram.push_back(picture[i + j * img_size * img_size + k * img_size]);
			}
		}
	}
	
	for(int i = 3; i < img_size; i++)
	{
		for(int j = 0; j < img_size; j++)
		{
			for(int k = 0; k < num_of_channels; k++)
			{
				temp_ram.push_back(picture[j + k * img_size * img_size + i * img_size]);
			}
		}
	}

	picture.clear();
	for(int i = 0; i < temp_ram.size(); i++) picture.push_back(temp_ram[i]);	
}

void tb_hw::flatten(vector4D source_vector,vector2D &dest_vector,int img_size, int num_of_channels)
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

void tb_hw::transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels)
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

void tb_hw::transform_4D_to_1D(vector4D source_vector,vector1D& dest_vector,int img_size, int num_of_channels)
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

