#ifndef HW_HPP_
#define HW_HPP_

#include <systemc>

class hw : public sc_core::sc_foreign_module
{
public:
	hw(sc_core::sc_module_name name) :
		sc_core::sc_foreign_module(name),
		axis_s_data_in("axis_s_data_in"),
		axis_s_valid("axis_s_valid"),
		axis_s_last("axis_s_last"),
	       	axis_s_ready("axis_s_ready"),
	       	axis_s_tkeep("axis_s_tkeep"),
	        axim_s_data("axim_s_data"),
		axim_s_valid("axim_s_valid"),
		axim_s_last("axim_s_last"),
        	axim_s_ready("axim_s_ready"),
        	axim_s_tkeep("axim_s_tkeep"),
        	s00_axi_aclk("s00_axi_aclk"),
        	s00_axi_aresetn("s00_axi_aresetn"),
        	s00_axi_awaddr("s00_axi_awaddr"),
        	s00_axi_awprot("s00_axi_awprot"),
        	s00_axi_awvalid("s00_axi_awvalid"),
        	s00_axi_awready("s00_axi_awready"),
        	s00_axi_wdata("s00_axi_wdata"),
        	s00_axi_wstrb("s00_axi_wstrb"),
        	s00_axi_wvalid("s00_axi_wvalid"),
        	s00_axi_wready("s00_axi_wready"),
        	s00_axi_bresp("s00_axi_bresp"),
        	s00_axi_bvalid("s00_axi_bvalid"),
        	s00_axi_bready("s00_axi_bready"),
        	s00_axi_araddr("s00_axi_araddr"),
        	s00_axi_arprot("s00_axi_arprot"),
        	s00_axi_arvalid("s00_axi_arvalid"),
        	s00_axi_arready("s00_axi_arready"),
        	s00_axi_rdata("s00_axi_rdata"),
        	s00_axi_rresp("s00_axi_rresp"),
        	s00_axi_rvalid("s00_axi_rvalid"),
        	s00_axi_rready("s00_axi_rready"),
		interupt_done("interupt_done")
	{

	}

	sc_core::sc_out< sc_dt::sc_logic > interupt_done;
    	

   	// AXI_STREAM MASTER interface signals
	sc_core::sc_out< sc_dt::sc_logic > axim_s_valid;
    	sc_core::sc_out< sc_dt::sc_logic > axim_s_last;
    	sc_core::sc_in< sc_dt::sc_logic > axim_s_ready;
    	sc_core::sc_out< sc_dt::sc_lv<16> > axim_s_data;
    	sc_core::sc_out< sc_dt::sc_lv<2> > axim_s_tkeep;
// AXI_STREAM SLAVE interface signals
    	sc_core::sc_in< sc_dt::sc_lv<16> > axis_s_data_in;
	sc_core::sc_in< sc_dt::sc_logic > axis_s_valid;
    	sc_core::sc_in< sc_dt::sc_logic > axis_s_last;
    	sc_core::sc_out< sc_dt::sc_logic > axis_s_ready;
	sc_core::sc_in< sc_dt::sc_lv<2> > axis_s_tkeep;
	// AXI_LITE interface
	sc_core::sc_in< bool > s00_axi_aclk;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_aresetn;
	sc_core::sc_in< sc_dt::sc_lv<4> > s00_axi_awaddr;
	sc_core::sc_in< sc_dt::sc_lv<3> > s00_axi_awprot;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_awvalid;
	sc_core::sc_out< sc_dt::sc_logic > s00_axi_awready;
	sc_core::sc_in< sc_dt::sc_lv<32> > s00_axi_wdata;
	sc_core::sc_in< sc_dt::sc_lv<4> > s00_axi_wstrb;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_wvalid;
	sc_core::sc_out< sc_dt::sc_logic > s00_axi_wready;
	sc_core::sc_out< sc_dt::sc_lv<2> > s00_axi_bresp;
	sc_core::sc_out< sc_dt::sc_logic > s00_axi_bvalid;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_bready;
	sc_core::sc_in< sc_dt::sc_lv<4> > s00_axi_araddr;
	sc_core::sc_in< sc_dt::sc_lv<3> > s00_axi_arprot;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_arvalid;
	sc_core::sc_out< sc_dt::sc_logic > s00_axi_arready;
	sc_core::sc_out< sc_dt::sc_lv<32> > s00_axi_rdata;
	sc_core::sc_out< sc_dt::sc_lv<2> > s00_axi_rresp;
	sc_core::sc_out< sc_dt::sc_logic > s00_axi_rvalid;
	sc_core::sc_in< sc_dt::sc_logic > s00_axi_rready;
	


	const char* hdl_name() const { return "cnn_ip_v1_0"; }
};

#endif
