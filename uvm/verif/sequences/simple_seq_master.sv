`ifndef simple_seq_master_SV
`define simple_seq_master_SV

class simple_seq_master extends base_seq_master;

    `uvm_object_utils(simple_seq_master)

    integer i = 0;
    seq_item_master item;
    integer param_to_send;
    

    function new(string name = "simple_seq_master");
      super.new(name);
      param_to_send = 0;
    endfunction

    virtual task body(); 
        item = seq_item_master::type_id::create("item");
        
        while(i < param_to_send) begin
            start_item(item); 
                assert(item.randomize());
                
                if(item.axim_s_ready == 1) begin
                    i = i+1;     
                end
            finish_item(item);
        end
        //$display("vrednost iteracije i je i = %d",i); 
        i = 0;

        start_item(item);
            item.axim_s_ready = 1'b0; 
        finish_item(item);
    
    endtask : body

endclass : simple_seq_master
`endif
