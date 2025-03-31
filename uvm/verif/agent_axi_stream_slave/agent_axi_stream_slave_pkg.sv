`ifndef AGENT_AXI_STREAM_SLAVE_PKG
`define AGENT_AXI_STREAM_SLAVE_PKG

package agent_axi_stream_slave_pkg;

    import uvm_pkg::*;
    `include "uvm_macros.svh"

    import configurations_pkg::*;   

    `include "seq_item_slave.sv"
    `include "sequencer_axi_stream_slave.sv"
    `include "driver_axi_stream_slave.sv"
    `include "monitor_axi_stream_slave.sv"
    `include "agent_axi_stream_slave.sv"

endpackage

`endif