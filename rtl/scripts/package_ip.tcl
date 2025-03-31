set root_dir [pwd]/..
set result_dir $root_dir/result
set release_dir $root_dir/release
file mkdir $result_dir/
file mkdir $release_dir/
set project_name "cnn_ip"
set design_name "design_cnn"
set jobs_num 2

create_project $project_name $result_dir -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]
set_property target_language VHDL [current_project]

# ===========================================
# ADDING SOURCES
# ===========================================

add_files -norecurse ${root_dir}/dut_hdl/add_tree.vhd
add_files -norecurse ${root_dir}/dut_hdl/BRAM_in_pic.vhd
add_files -norecurse ${root_dir}/dut_hdl/BRAM_out_pic.vhd
add_files -norecurse ${root_dir}/dut_hdl/cache_block_picture.vhd
add_files -norecurse ${root_dir}/dut_hdl/cache_block_weights.vhd
add_files -norecurse ${root_dir}/dut_hdl/cnn_ip_v1_0.vhd
add_files -norecurse ${root_dir}/dut_hdl/cnn_ip_v1_0_S00_AXI.vhd
add_files -norecurse ${root_dir}/dut_hdl/controlpath.vhd
add_files -norecurse ${root_dir}/dut_hdl/datapath_cnn.vhd
add_files -norecurse ${root_dir}/dut_hdl/line_fifo_buffer.vhd
add_files -norecurse ${root_dir}/dut_hdl/MAC.vhd
add_files -norecurse ${root_dir}/dut_hdl/MAC_top.vhd
add_files -norecurse ${root_dir}/dut_hdl/TOP_cnn.vhd

# ===========================================
# IP PACKAGING
# ===========================================
ipx::package_project -root_dir $result_dir -vendor user.org -library user -taxonomy /UserIP

set_property vendor FTN [ipx::current_core]
set_property name AXI_CNN [ipx::current_core]
set_property display_name AXI_CNN_v1_0 [ipx::current_core]
set_property vendor_display_name FTN [ipx::current_core]
set_property supported_families {zynq Production} [ipx::current_core]

ipx::add_bus_interface m_axis [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:axis_rtl:1.0 [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:axis:1.0 [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property interface_mode master [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
ipx::add_port_map TDATA [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property physical_name axim_s_data [ipx::get_port_maps TDATA -of_objects [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TVALID [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property physical_name axim_s_valid [ipx::get_port_maps TVALID -of_objects [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TLAST [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property physical_name axim_s_last [ipx::get_port_maps TLAST -of_objects [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TKEEP [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property physical_name axim_s_tkeep [ipx::get_port_maps TKEEP -of_objects [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TREADY [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]
set_property physical_name axim_s_ready [ipx::get_port_maps TREADY -of_objects [ipx::get_bus_interfaces m_axis -of_objects [ipx::current_core]]]

ipx::add_bus_interface s_axis [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:axis_rtl:1.0 [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:axis:1.0 [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
ipx::add_port_map TDATA [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property physical_name axis_s_data_in [ipx::get_port_maps TDATA -of_objects [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TLAST [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property physical_name axis_s_last [ipx::get_port_maps TLAST -of_objects [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TVALID [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property physical_name axis_s_valid [ipx::get_port_maps TVALID -of_objects [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TKEEP [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property physical_name axis_s_tkeep [ipx::get_port_maps TKEEP -of_objects [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]]
ipx::add_port_map TREADY [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]
set_property physical_name axis_s_ready [ipx::get_port_maps TREADY -of_objects [ipx::get_bus_interfaces s_axis -of_objects [ipx::current_core]]]

ipx::infer_bus_interface interupt_done xilinx.com:signal:interrupt_rtl:1.0 [ipx::current_core]

set new_value "s00_axi:m_axis:s_axis"
set bus_params [ipx::get_bus_parameters ASSOCIATED_BUSIF -of_objects [ipx::get_bus_interfaces s00_axi_aclk -of_objects [ipx::current_core]]]
set_property value $new_value $bus_params
ipx::merge_project_changes files [ipx::current_core]


# ===========================================
# PACKAGE IP BUTTON -- DOESN'T WORK
# ===========================================

set_property core_revision 1 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]

ipx::save_core [ipx::current_core]
set_property ip_repo_paths ../result/ [current_project]
update_ip_catalog
ipx::check_integrity -quiet [ipx::current_core]
ipx::archive_core $result_dir/cnn_ip_v1_0.zip [ipx::current_core]
# close_project

# ===========================================
# CREATE BLOCK DESIGN
# ===========================================
create_bd_design $design_name
update_compile_order -fileset sources_1

startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup
startgroup
set_property -dict [list CONFIG.PCW_USE_S_AXI_HP0 {1} CONFIG.PCW_USE_FABRIC_INTERRUPT {1} CONFIG.PCW_IRQ_F2P_INTR {1} CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {130}] [get_bd_cells processing_system7_0]
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma_0
endgroup
startgroup
create_bd_cell -type ip -vlnv FTN:user:AXI_CNN:1.0 AXI_CNN_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0
endgroup

set_property ip_repo_paths ../result/ [current_project]
set_property -dict [list CONFIG.c_s_axis_s2mm_tdata_width.VALUE_SRC USER] [get_bd_cells axi_dma_0]
set_property -dict [list CONFIG.c_include_sg {0} CONFIG.c_sg_length_width {26} CONFIG.c_sg_include_stscntrl_strm {0} CONFIG.c_m_axis_mm2s_tdata_width {16} CONFIG.c_s_axis_s2mm_tdata_width {16}] [get_bd_cells axi_dma_0]
set_property -dict [list CONFIG.NUM_PORTS {3}] [get_bd_cells xlconcat_0]

connect_bd_intf_net [get_bd_intf_pins axi_dma_0/M_AXIS_MM2S] [get_bd_intf_pins AXI_CNN_0/s_axis]
connect_bd_intf_net [get_bd_intf_pins AXI_CNN_0/m_axis] [get_bd_intf_pins axi_dma_0/S_AXIS_S2MM]
connect_bd_net [get_bd_pins processing_system7_0/FCLK_CLK0] [get_bd_pins processing_system7_0/M_AXI_GP0_ACLK]
connect_bd_net [get_bd_pins axi_dma_0/mm2s_introut] [get_bd_pins xlconcat_0/In0]
connect_bd_net [get_bd_pins axi_dma_0/s2mm_introut] [get_bd_pins xlconcat_0/In1]
connect_bd_net [get_bd_pins AXI_CNN_0/interupt_done] [get_bd_pins xlconcat_0/In2]
connect_bd_net [get_bd_pins xlconcat_0/dout] [get_bd_pins processing_system7_0/IRQ_F2P]

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/processing_system7_0/FCLK_CLK0 (130 MHz)} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_dma_0/S_AXI_LITE} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins axi_dma_0/S_AXI_LITE]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/axi_dma_0/M_AXI_MM2S} Slave {/processing_system7_0/S_AXI_HP0} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins processing_system7_0/S_AXI_HP0]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {/processing_system7_0/FCLK_CLK0 (130 MHz)} Clk_xbar {/processing_system7_0/FCLK_CLK0 (130 MHz)} Master {/axi_dma_0/M_AXI_S2MM} Slave {/processing_system7_0/S_AXI_HP0} ddr_seg {Auto} intc_ip {/axi_mem_intercon} master_apm {0}}  [get_bd_intf_pins axi_dma_0/M_AXI_S2MM]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/processing_system7_0/FCLK_CLK0 (130 MHz)} Clk_slave {Auto} Clk_xbar {/processing_system7_0/FCLK_CLK0 (130 MHz)} Master {/processing_system7_0/M_AXI_GP0} Slave {/AXI_CNN_0/S00_AXI} ddr_seg {Auto} intc_ip {/ps7_0_axi_periph} master_apm {0}}  [get_bd_intf_pins AXI_CNN_0/S00_AXI]
#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {/processing_system7_0/FCLK_CLK0 (130 MHz)} Clk_xbar {/processing_system7_0/FCLK_CLK0 (130 MHz)} Master {/axi_dma_0/M_AXI_S2MM} Slave {/processing_system7_0/S_AXI_HP0} ddr_seg {Auto} intc_ip {/axi_mem_intercon} master_apm {0}}  [get_bd_intf_pins axi_dma_0/M_AXI_S2MM]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/processing_system7_0/FCLK_CLK0 (125 MHz)} Clk_slave {/processing_system7_0/FCLK_CLK0 (125 MHz)} Clk_xbar {/processing_system7_0/FCLK_CLK0 (125 MHz)} Master {/axi_dma_0/M_AXI_S2MM} Slave {/processing_system7_0/S_AXI_HP0} ddr_seg {Auto} intc_ip {/axi_mem_intercon} master_apm {0}}  [get_bd_intf_pins axi_dma_0/M_AXI_S2MM]

regenerate_bd_layout
assign_bd_address
save_bd_design


# ===========================================
# MAKE WRAPPER
# ===========================================


make_wrapper -files [get_files $result_dir/$project_name.srcs/sources_1/bd/$design_name/$design_name.bd] -top
add_files -norecurse $result_dir/$project_name.srcs/sources_1/bd/$design_name/hdl/${design_name}_wrapper.vhd
update_compile_order -fileset sources_1
set_property top design_cnn_wrapper [current_fileset] 
update_compile_order -fileset sources_1

# ===========================================
# SYNTHESIS
# ===========================================

 launch_runs synth_1 -jobs $jobs_num
 wait_on_run synth_1

# # ===========================================
# # IMPLEMENTATION
# # ===========================================

launch_runs impl_1 -jobs 2
wait_on_run impl_1


# # ===========================================
# # GENERATE BITSREAM
# # ===========================================
launch_runs impl_1 -to_step write_bitstream -jobs 2
wait_on_run impl_1
# # ===========================================
# # COPY BITSTREAM TO RELEASE FOLDER
# # ===========================================

file copy -force $result_dir/$project_name.runs/impl_1/${design_name}_wrapper.bit $release_dir/${design_name}_wrapper.bit
