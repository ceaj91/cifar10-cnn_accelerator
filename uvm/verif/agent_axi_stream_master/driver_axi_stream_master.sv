`ifndef driver_axi_stream_master_SV
`define driver_axi_stream_master_SV

class driver_axi_stream_master extends uvm_driver#(seq_item_master);

   `uvm_component_utils(driver_axi_stream_master)

    //int num = 1;
    seq_item_master item;
    virtual interface cnn_interface vif;

    function new(string name = "driver", uvm_component parent = null);
        super.new(name,parent);
        
        if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set for: ",get_full_name(),".vif"})
            
        item = seq_item_master::type_id::create("item");
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
      
    endfunction : connect_phase

    task main_phase(uvm_phase phase);
        
        
        @(posedge vif.clk);

        forever begin
            @(posedge vif.clk);
                
            seq_item_port.get_next_item(item);
                vif.axim_s_ready = item.axim_s_ready;
                //@(posedge vif.clk iff vif.end_command_int);           
            seq_item_port.item_done();
        end
    endtask : main_phase


endclass : driver_axi_stream_master

`endif
