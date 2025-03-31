`ifndef AGENT_AXI_LITE_PKG
`define AGENT_AXI_LITE_PKG

package agent_axi_lite_pkg;

    import uvm_pkg::*;
    `include "uvm_macros.svh"

    import configurations_pkg::*;   

    `include "seq_item_lite.sv"
    `include "sequencer_axi_lite.sv"
    `include "driver_axi_lite.sv"
    `include "monitor_axi_lite.sv"
    `include "agent_axi_lite.sv"

endpackage

`endif