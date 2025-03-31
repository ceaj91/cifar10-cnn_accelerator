`ifndef CNN_CONFIG_SV
`define CNN_CONFIG_SV

class cnn_config extends uvm_object;

   uvm_active_passive_enum is_active = UVM_ACTIVE;
   
   `uvm_object_utils_begin (cnn_config)
      `uvm_field_enum(uvm_active_passive_enum, is_active, UVM_DEFAULT)
   `uvm_object_utils_end

   function new(string name = "cnn_config");
      super.new(name);
   endfunction

endclass : cnn_config
`endif