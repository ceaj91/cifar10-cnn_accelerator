`ifndef SEQ_PKG_SV
`define SEQ_PKG_SV

package seq_pkg;

    import uvm_pkg::*;      // import the UVM library
    `include "uvm_macros.svh" // Include the UVM macros

    import agent_axi_stream_slave_pkg::seq_item_slave;
    import agent_axi_stream_master_pkg::seq_item_master;
    import agent_axi_lite_pkg::seq_item_lite;

    import agent_axi_stream_slave_pkg::sequencer_axi_stream_slave;
    import agent_axi_stream_master_pkg::sequencer_axi_stream_master;
    import agent_axi_lite_pkg::sequencer_axi_lite;

    `include "base_seq_lite.sv"
    `include "base_seq_master.sv"
    `include "base_seq_slave.sv"

    `include "simple_seq_lite.sv"
    `include "simple_seq_slave.sv"
    `include "simple_seq_master.sv"

endpackage 
`endif