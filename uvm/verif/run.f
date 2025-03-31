-uvmhome "/eda/cadence/2019-20/RHELx86/XCELIUM_19.03.013/tools/methodology/UVM/CDNS-1.2/" 
-uvm +UVM_TESTNAME=test_xcelium +UVM_VERBOSITY=UVM_LOW
-sv +incdir+../verif
-sv +incdir+../verif/agent_axi_lite
-sv +incdir+../verif/agent_axi_stream_master
-sv +incdir+../verif/agent_axi_stream_slave
-sv +incdir+../verif/sequences
-sv +incdir+../verif/configuration
 
 ../../rtl/dut_hdl/add_tree.vhd
 ../../rtl/dut_hdl/BRAM_in_pic.vhd
 ../../rtl/dut_hdl/BRAM_out_pic.vhd
 ../../rtl/dut_hdl/cache_block_picture.vhd
 ../../rtl/dut_hdl/cache_block_weights.vhd
 ../../rtl/dut_hdl/cnn_ip_v1_0.vhd
 ../../rtl/dut_hdl/cnn_ip_v1_0_S00_AXI.vhd
 ../../rtl/dut_hdl/controlpath.vhd
 ../../rtl/dut_hdl/datapath_cnn.vhd
 ../../rtl/dut_hdl/line_fifo_buffer.vhd
 ../../rtl/dut_hdl/MAC.vhd
 ../../rtl/dut_hdl/MAC_top.vhd
 ../../rtl/dut_hdl/TOP_cnn.vhd

-sv ../verif/configuration/configuration_pkg.sv
-sv ../verif/agent_axi_lite/agent_axi_lite_pkg.sv
-sv ../verif/agent_axi_stream_master/agent_axi_stream_master_pkg.sv
-sv ../verif/agent_axi_stream_slave/agent_axi_stream_slave_pkg.sv
-sv ../verif/sequences/seq_pkg_xcelium.sv
-sv ../verif/test_pkg_xcelium.sv

-sv ../verif/interface.sv
-sv ../verif/top.sv

#-LINEDEBUG
-access +rwc
-disable_sem2009
-svseed random
-nowarn "MEMODR"
-timescale 1ns/10ps
