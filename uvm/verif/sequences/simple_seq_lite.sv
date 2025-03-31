`ifndef simple_seq_lite_SV
`define simple_seq_lite_SV

class simple_seq_lite extends base_seq_lite;

    `uvm_object_utils(simple_seq_lite)

    logic[31:0] command_global;
    integer reset_flag;
    seq_item_lite item;
    

    function new(string name = "simple_seq_lite");
      super.new(name);
      
    endfunction

    virtual task body();   
        item = seq_item_lite::type_id::create("item");
        `uvm_info(get_name(), $sformatf("SENDING COMMAND"),   UVM_HIGH) 

        start_item(item);
        item.COMMAND = command_global;
        item.offset = 0;
        finish_item(item);
        `uvm_info(get_name(), $sformatf("COMMAND SENT %h",command_global),   UVM_HIGH) 
    endtask : body

endclass : simple_seq_lite
`endif
