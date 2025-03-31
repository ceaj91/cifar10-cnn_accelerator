`ifndef seq_item_lite_SV
`define seq_item_lite_SV

class seq_item_lite extends uvm_sequence_item;

    //AXI LITE
    logic [31:0] COMMAND;
    integer offset;
    
    `uvm_object_utils_begin(seq_item_lite)
            `uvm_field_int(COMMAND,  UVM_DEFAULT)
            `uvm_field_int(offset,    UVM_DEFAULT)
    `uvm_object_utils_end

    
    function new (string name = "seq_item_lite");
       super.new(name);
    endfunction // new

    //constraint axim_s_ready_constraint  { axis_s_ready   inside { 0, 1 }; }
    /*
      constraint COMMAND_constraint { COMMAND  inside { 32'h00000000, 
                                                        32'h00000001, 
                                                        32'h00000002, 
                                                        32'h00000004, 
                                                        32'h00000008, 
                                                        32'h00000010, 
                                                        32'h00000020, 
                                                        32'h00000040, 
                                                        32'h00000080, 
                                                        32'h00000100, 
                                                        32'h00000200, 
                                                        32'h00000400, 
                                                        32'h00000800, 
                                                        32'h00001000,
                                                        32'h00002000};}
    */
endclass : seq_item_lite

`endif 