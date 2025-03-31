`ifndef driver_axi_stream_slave_SV
`define driver_axi_stream_slave_SV

class driver_axi_stream_slave extends uvm_driver#(seq_item_slave);

   `uvm_component_utils(driver_axi_stream_slave)

    //int num = 1;
    seq_item_slave item;
    virtual interface cnn_interface vif;

    function new(string name = "driver_axi_stream_slave", uvm_component parent = null);
        super.new(name,parent);
        
        if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set for: ",get_full_name(),".vif"})
            
        item = seq_item_slave::type_id::create("item");
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
      
    endfunction : connect_phase

    task main_phase(uvm_phase phase);
        

        forever begin
            @(posedge vif.clk);
                
            seq_item_port.get_next_item(item);
                while(vif.axis_s_ready == 0) begin
                    @(posedge vif.clk);                       //cekaj dok ne bude ip spreman da primi podatke 
                end
                vif.axis_s_valid    = item.axis_s_valid;
                vif.axis_s_last     = item.axis_s_last;
                vif.axis_s_data_in  = item.axis_s_data_in;     
                
                if(item.axis_s_last == 1)begin 
                     @(posedge vif.clk); 
                     vif.axis_s_last = 1'b0;
                     vif.axis_s_valid = 1'b0;

                end


                        
                   
            seq_item_port.item_done();
        end
    endtask : main_phase


endclass : driver_axi_stream_slave

`endif
