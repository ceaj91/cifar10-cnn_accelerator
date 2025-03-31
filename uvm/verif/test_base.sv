`ifndef TEST_BASE_SV
`define TEST_BASE_SV

class test_base extends uvm_test;

    `uvm_component_utils(test_base)

    cnn_env env;
    cnn_config cfg;

    function new(string name = "test_base", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new


    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        cfg = cnn_config::type_id::create("cfg");      
        uvm_config_db#(cnn_config)::set(this, "*", "cnn_config", cfg);      
        env = cnn_env::type_id::create("env", this);      
    endfunction : build_phase


    function void end_of_elaboration_phase(uvm_phase phase);
        super.end_of_elaboration_phase(phase);
        uvm_top.print_topology();
    endfunction : end_of_elaboration_phase

endclass : test_base

`endif