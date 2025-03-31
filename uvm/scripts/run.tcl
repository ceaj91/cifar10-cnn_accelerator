# ===================================================================================
# Definisanje direktorijuma u kojem ce biti projekat
# ===================================================================================
cd ..
set root_dir [pwd]
cd scripts
set resultDir ../vivado_project

file mkdir $resultDir

create_project cnn_3_agent $resultDir -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.1 [current_project]

# ===================================================================================
# Ukljucivanje svih izvornih i simulacionih fajlova u projekat
# ===================================================================================
add_files -norecurse ../../rtl/dut_hdl/add_tree.vhd
add_files -norecurse ../../rtl/dut_hdl/BRAM_in_pic.vhd
add_files -norecurse ../../rtl/dut_hdl/BRAM_out_pic.vhd
add_files -norecurse ../../rtl/dut_hdl/cache_block_picture.vhd
add_files -norecurse ../../rtl/dut_hdl/cache_block_weights.vhd
add_files -norecurse ../../rtl/dut_hdl/cnn_ip_v1_0.vhd
add_files -norecurse ../../rtl/dut_hdl/cnn_ip_v1_0_S00_AXI.vhd
add_files -norecurse ../../rtl/dut_hdl/controlpath.vhd
add_files -norecurse ../../rtl/dut_hdl/datapath_cnn.vhd
add_files -norecurse ../../rtl/dut_hdl/line_fifo_buffer.vhd
add_files -norecurse ../../rtl/dut_hdl/MAC.vhd
add_files -norecurse ../../rtl/dut_hdl/MAC_top.vhd
add_files -norecurse ../../rtl/dut_hdl/TOP_cnn.vhd

update_compile_order -fileset sources_1

set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/agent_axi_lite/agent_axi_lite_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/agent_axi_stream_master/agent_axi_stream_master_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/agent_axi_stream_slave/agent_axi_stream_slave_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/configuration/configuration_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/sequences/seq_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/test_pkg.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/interface.sv
set_property SOURCE_SET sources_1 [get_filesets sim_1]
add_files -fileset sim_1 -norecurse ../verif/top.sv

update_compile_order -fileset sources_1
update_compile_order -fileset sim_1

# ===================================================================================
# Ukljucivanje uvm biblioteke
# ===================================================================================
set_property -name {xsim.compile.xvlog.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.elaborate.xelab.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.simulate.xsim.more_options} -value {-testplusarg UVM_TESTNAME=test_simple1 -testplusarg UVM_VERBOSITY=UVM_LOW -sv_seed 5} -objects [get_filesets sim_1]