#ifndef TB_HW
#define TB_HW

#include <systemc>
#include <vector>
#include <array>
#include <algorithm>
#include "hw.hpp"
#include "../specification/cpp_implementation/MaxPoolLayer.hpp"
#include "../specification/cpp_implementation/denselayer.hpp"
#include "../vp/TLM/addresses.hpp"


#define IDLE_CMD 0x00000000
#define LOAD_BIAS_CMD 0x00000001
#define LOAD_WEIGHTS_0_CMD 0x00000002
#define LOAD_PICTURE_0_CMD 0x00000004
#define DO_CONV_0_CMD 0x00000008
#define LOAD_WEIGHTS_1_CMD 0x00000010
#define LOAD_PICTURE_1_CMD 0x00000020
#define DO_CONV_1_CMD 0x00000040
#define LOAD_WEIGHTS_2_CMD 0x00000080
#define LOAD_PICTURE_2_CMD 0x00000100
#define DO_CONV_2_CMD 0x00000200
#define RESET_CMD 0x00000400
#define SEND_OUTPUT_FROM_CONV_0_CMD 0x00000800
#define SEND_OUTPUT_FROM_CONV_1_CMD 0x00001000
#define SEND_OUTPUT_FROM_CONV_2_CMD 0x00002000
using namespace std;
typedef vector<vector<vector<vector<float>>>> vector4D;
typedef vector<vector<vector<float>>> vector3D;
typedef vector<vector<float>> vector2D;
typedef vector<float> vector1D;

class tb_hw : public sc_core::sc_module
{
public:
	tb_hw(sc_core::sc_module_name name);
protected:
	void gen_thread();
	void mon_thread();
	hw dut;
	sc_core::sc_clock clk;
	
	sc_core::sc_signal< sc_dt::sc_lv<16> > axis_s_data_in;
	sc_core::sc_signal< sc_dt::sc_logic > axis_s_valid;
    	sc_core::sc_signal< sc_dt::sc_logic > axis_s_last;
    	sc_core::sc_signal< sc_dt::sc_logic > axis_s_ready;
	sc_core::sc_signal< sc_dt::sc_lv<2> > axis_s_tkeep;

   	// AXI_STREAM MASTER interface signals
    	sc_core::sc_signal< sc_dt::sc_lv<16> > axim_s_data;
	sc_core::sc_signal< sc_dt::sc_logic > axim_s_valid;
    	sc_core::sc_signal< sc_dt::sc_logic > axim_s_last;
    	sc_core::sc_signal< sc_dt::sc_logic > axim_s_ready;
    	sc_core::sc_signal< sc_dt::sc_lv<2> > axim_s_tkeep;

	// AXI_LITE interface
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_aclk;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_aresetn;
	sc_core::sc_signal< sc_dt::sc_lv<4> > s00_axi_awaddr;
	sc_core::sc_signal< sc_dt::sc_lv<3> > s00_axi_awprot;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_awvalid;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_awready;
	sc_core::sc_signal< sc_dt::sc_lv<32> > s00_axi_wdata;
	sc_core::sc_signal< sc_dt::sc_lv<4> > s00_axi_wstrb;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_wvalid;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_wready;
	sc_core::sc_signal< sc_dt::sc_lv<2> > s00_axi_bresp;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_bvalid;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_bready;
	sc_core::sc_signal< sc_dt::sc_lv<4> > s00_axi_araddr;
	sc_core::sc_signal< sc_dt::sc_lv<3> > s00_axi_arprot;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_arvalid;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_arready;
	sc_core::sc_signal< sc_dt::sc_lv<32> > s00_axi_rdata;
	sc_core::sc_signal< sc_dt::sc_lv<2> > s00_axi_rresp;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_rvalid;
	sc_core::sc_signal< sc_dt::sc_logic > s00_axi_rready;

	sc_core::sc_signal< sc_dt::sc_logic > interupt_done;

	void pad_img(int img_size, int num_of_channels);
	void format_image(int img_size, int num_of_channels);
	void transform_1D_to_4D(vector1D input_vector, vector4D& output_vector, int img_size, int num_of_channels);
	void transform_4D_to_1D(vector4D source_vector,vector1D& dest_vector,int img_size, int num_of_channels);
	void flatten(vector4D source_vector,vector2D &dest_vector,int img_size, int num_of_channels);

	MaxPoolLayer *maxpool[3];
	DenseLayer *dense_layer[2];
	std::vector<uint16_t> bias;
	std::vector<uint16_t> weights;
	std::vector<uint16_t> picture;
	vector1D image;
	vector4D image4D;
	//vector<hwdata_t> orig_img;
	vector4D output;
	vector2D dense1_input;
	vector2D dense1_output;
	vector2D dense2_output;
private:
};

#ifndef SC_MAIN
SC_MODULE_EXPORT(tb_hw)
#endif

#endif
