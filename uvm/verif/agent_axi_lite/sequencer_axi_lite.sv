`ifndef sequencer_axi_lite_SV
`define sequencer_axi_lite_SV

class sequencer_axi_lite extends uvm_sequencer#(seq_item_lite);

   `uvm_component_utils(sequencer_axi_lite)
   
   function new(string name = "sequencer_axi_lite", uvm_component parent = null);
      super.new(name,parent);
   endfunction

endclass : sequencer_axi_lite

`endif