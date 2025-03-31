`ifndef seq_item_master_SV
`define seq_item_master_SV

class seq_item_master extends uvm_sequence_item;

    
    //AXI STREAM MASTER
        
    logic[15:0] axim_s_data[$];
    logic axim_s_valid;
    logic axim_s_last;
    rand logic axim_s_ready;

    `uvm_object_utils_begin(seq_item_master)
            //`uvm_field_int(axim_s_data,  UVM_DEFAULT)
            `uvm_field_int(axim_s_valid,    UVM_DEFAULT)
            `uvm_field_int(axim_s_last,     UVM_DEFAULT)
    `uvm_object_utils_end

    function new (string name = "seq_item_master");
       super.new(name);
    endfunction // new

    constraint axim_s_ready_constraint   { axim_s_ready dist {1'b0 := 5, 1'b1 := 95};}   

endclass : seq_item_master

`endif 

