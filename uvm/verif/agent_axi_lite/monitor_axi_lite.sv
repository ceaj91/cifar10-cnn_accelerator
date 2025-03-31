`ifndef monitor_axi_lite_SV
`define monitor_axi_lite_SV

class monitor_axi_lite extends uvm_monitor;

    `uvm_component_utils(monitor_axi_lite)

    cnn_config cfg;
    seq_item_lite item;
    virtual interface cnn_interface vif;
    uvm_analysis_port #(seq_item_lite) port_axi_lite; //sends items to scoreboard

    covergroup read_axi_lite;
        option.per_instance = 1;
        cover_input_command: coverpoint vif.s00_axi_wdata{
            bins LOAD_BIAS_CMD       = {32'h00000001};
            bins LOAD_WEIGHTS_0_CMD  = {32'h00000002};  
            bins LOAD_PICTURE_0_CMD  = {32'h00000004};  
            
            bins DO_CONV_0_CMD       = {32'h00000008};  
            bins LOAD_WEIGHTS_1_CMD  = {32'h00000010};  
            bins LOAD_PICTURE_1_CMD  = {32'h00000020};  
            
            bins DO_CONV_1_CMD       = {32'h00000040};
            bins LOAD_WEIGHTS_2_CMD  = {32'h00000080};
            bins LOAD_PICTURE_2_CMD  = {32'h00000100};
            
            bins DO_CONV_2_CMD       = {32'h00000200};
            
            bins RESET_CMD           = {32'h00000400};  
            
            bins SEND_OUTPUT_FROM_CONV_0_CMD = {32'h00000800};                                 
            bins SEND_OUTPUT_FROM_CONV_1_CMD = {32'h00001000};
            bins SEND_OUTPUT_FROM_CONV_2_CMD = {32'h00002000};
        }
    endgroup 

    function new(string name = "cnn_monitor", uvm_component parent = null);
        super.new(name,parent);  
        read_axi_lite = new();
        port_axi_lite = new("port_axi_lite", this);
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);

        if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
        
        if(!uvm_config_db#(cnn_config)::get(this, "", "cnn_config", cfg))
            `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})
    endfunction : connect_phase

    task main_phase(uvm_phase phase);
        item = seq_item_lite::type_id::create("item",this);
        
        forever begin
            @(negedge vif.clk iff (vif.s00_axi_awvalid && vif.s00_axi_wvalid)); // mozda neg edge
            @(posedge vif.clk iff vif.s00_axi_awready); //mozda neg edge
            // POS ILI NEGEDGE ???????
            
            read_axi_lite.sample();
            
            item.COMMAND = vif.s00_axi_wdata;
            port_axi_lite.write(item);
        end
    endtask : main_phase


endclass : monitor_axi_lite

`endif

//NAPRAVI OBJEKAT ZA COVERGROUP
