`ifndef seq_item_slave_SV
`define seq_item_slave_SV

class seq_item_slave extends uvm_sequence_item;


    //AXI STREAM SLAVE 
    logic [15 : 0] axis_s_data_in;
    rand logic axis_s_valid;            
    logic axis_s_last;    
    
    `uvm_object_utils_begin(seq_item_slave)
            `uvm_field_int(axis_s_data_in,  UVM_DEFAULT)
            `uvm_field_int(axis_s_valid,    UVM_DEFAULT)
            `uvm_field_int(axis_s_last,     UVM_DEFAULT)
    `uvm_object_utils_end

    
    function new (string name = "seq_item_slave");
       super.new(name);
    endfunction

    constraint axis_s_valid_constraint   { axis_s_valid dist {1'b0 := 5, 1'b1 := 95};}

endclass : seq_item_slave

`endif 