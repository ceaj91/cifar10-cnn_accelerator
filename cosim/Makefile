
CPP_SOURCES = sc_main.cpp tb_hw.cpp ../specification/cpp_implementation/MaxPoolLayer.cpp ../specification/cpp_implementation/denselayer.cpp

VHDL_SOURCES = ../rtl/dut_hdl/line_fifo_buffer.vhd ../rtl/dut_hdl/MAC.vhd ../rtl/dut_hdl/BRAM_out_pic.vhd ../rtl/dut_hdl/BRAM_in_pic.vhd ../rtl/dut_hdl/cache_block_picture.vhd ../rtl/dut_hdl/cache_block_weights.vhd ../rtl/dut_hdl/add_tree.vhd ../rtl/dut_hdl/MAC_top.vhd ../rtl/dut_hdl/datapath_cnn.vhd ../rtl/dut_hdl/controlpath.vhd ../rtl/dut_hdl/TOP_cnn.vhd ../rtl/dut_hdl/cnn_ip_v1_0_S00_AXI.vhd ../rtl/dut_hdl/cnn_ip_v1_0.vhd

INCLUDE_PATHS = -I../specification/cpp_implementation

XMSC_OPTIONS = -sc_main -DSC_INCLUDE_DYANMIC_PROCESSES -v93

all:
	xmsc_run $(XMSC_OPTIONS) $(INCLUDE_PATHS) $(CPP_SOURCES) $(VHDL_SOURCES) 
