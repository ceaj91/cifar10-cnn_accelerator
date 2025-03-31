`ifndef CNN_ENV_SV
`define CNN_ENV_SV

class cnn_env extends uvm_env;

   `uvm_component_utils(cnn_env)

   agent_axi_lite agent_lite;
   agent_axi_stream_master agent_master;
   agent_axi_stream_slave agent_slave;

   cnn_config cfg;
   cnn_scoreboard scoreboard;
   virtual interface cnn_interface vif;

   function new(string name = "cnn_env", uvm_component parent = null);
      super.new(name,parent);
   endfunction

   function void build_phase(uvm_phase phase);
      super.build_phase(phase);

      if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
        `uvm_fatal("NOVIF",{"virtual interface must be set:",get_full_name(),".vif"})
      if(!uvm_config_db#(cnn_config)::get(this, "", "cnn_config", cfg))
        `uvm_fatal("NOCONFIG",{"Config object must be set for: ",get_full_name(),".cfg"})

      //Setting to configuration database
      uvm_config_db#(cnn_config)::set(this, "*", "cnn_config", cfg);
      uvm_config_db#(virtual cnn_interface)::set(this, "*", "cnn_interface", vif);

      
      agent_lite = agent_axi_lite::type_id::create("agent_lite",this);
      agent_master = agent_axi_stream_master::type_id::create("agent_master",this);
      agent_slave = agent_axi_stream_slave::type_id::create("agent_slave",this);

      scoreboard = cnn_scoreboard::type_id::create("scoreboard",this);

   endfunction : build_phase

   function void connect_phase(uvm_phase phase);
      super.connect_phase(phase);
      
      agent_master.mon.port_axis.connect(scoreboard.port_axis);
      agent_lite.mon.port_axi_lite.connect(scoreboard.port_axi_lite);
      
   endfunction : connect_phase
   
endclass : cnn_env


`endif 