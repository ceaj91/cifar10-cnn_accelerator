`ifndef TEST_PKG_SV
`define TEST_PKG_SV

package test_pkg;

	import uvm_pkg::*;      // import the UVM library   
	`include "uvm_macros.svh" // Include the UVM macros

	import agent_axi_stream_slave_pkg::*;
	import agent_axi_lite_pkg::*;
	import agent_axi_stream_master_pkg::*;
	import seq_pkg::*;
	import configurations_pkg::*;   

	`include "scoreboard_xcelium.sv" 
	`include "environment.sv"   
	`include "test_base.sv"
	`include "test_simple1.sv"
	`include "test_simple2.sv"
	`include "test_simple3.sv"
	`include "test_xcelium.sv"


endpackage : test_pkg

`include "interface.sv"

`endif
