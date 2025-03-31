`ifndef agent_axi_stream_master_SV
`define agent_axi_stream_master_SV

class agent_axi_stream_master extends uvm_agent;

    cnn_config cfg;
    driver_axi_stream_master drv;
    sequencer_axi_stream_master seqr;
    monitor_axi_stream_master mon;

    virtual interface cnn_interface vif;

    `uvm_component_utils_begin(agent_axi_stream_master)
        `uvm_field_object(cfg, UVM_DEFAULT)
    `uvm_component_utils_end
    

    function new(string name = "agent_axi_stream_master", uvm_component parent = null);
        super.new(name,parent);
    endfunction

    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        
        //Getting from configuration database
        if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
        if(!uvm_config_db#(cnn_config)::get(this, "", "cnn_config", cfg))
            `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})    
        
        //Setting to configuration database
        uvm_config_db#(cnn_config)::set(this, "*", "cnn_config", cfg);
        uvm_config_db#(virtual cnn_interface)::set(this, "*", "cnn_interface", vif);


        mon = monitor_axi_stream_master::type_id::create("mon", this);
        
        if(cfg.is_active == UVM_ACTIVE) begin    
            drv = driver_axi_stream_master::type_id::create("drv", this);
            seqr = sequencer_axi_stream_master::type_id::create("seqr", this);
        end
    endfunction: build_phase

    function void connect_phase(uvm_phase phase);
    super.connect_phase(phase);
    if(cfg.is_active == UVM_ACTIVE) begin
        drv.seq_item_port.connect(seqr.seq_item_export);
    end
   endfunction : connect_phase
endclass : agent_axi_stream_master

`endif