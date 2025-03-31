`ifndef AGENT_AXI_STREAM_MASTER_PKG
`define AGENT_AXI_STREAM_MASTER_PKG

package agent_axi_stream_master_pkg;

    import uvm_pkg::*;
    `include "uvm_macros.svh"

    import configurations_pkg::*;   

    `include "seq_item_master.sv"
    `include "sequencer_axi_stream_master.sv"
    `include "driver_axi_stream_master.sv"
    `include "monitor_axi_stream_master.sv"
    `include "agent_axi_stream_master.sv"

endpackage

`endif