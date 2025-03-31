`ifndef base_seq_master_SV
`define base_seq_master_SV

class base_seq_master extends uvm_sequence#(seq_item_master);

    `uvm_object_utils(base_seq_master)
    `uvm_declare_p_sequencer(sequencer_axi_stream_master)


    function new(string name = "base_seq_master");
        super.new(name);
    endfunction

    // objections are raised in pre_body
    virtual task pre_body();
        uvm_phase phase = get_starting_phase();
           
            if (phase != null)
            phase.raise_objection(this, {"Running sequence '", get_full_name(), "'"});
    endtask : pre_body

    // objections are dropped in post_body
    virtual task post_body();
        uvm_phase phase = get_starting_phase();
            
            if (phase != null)
            phase.drop_objection(this, {"Completed sequence '", get_full_name(), "'"});
    endtask : post_body


    

endclass : base_seq_master

`endif

/*
1. Relativna putanja fajlova - nisam uspeo da realizujem
2. Zasto ne mogu da printam brojeve van funkcije
3. Funkcija iz real u logic[15:0], zapis 4.12
*/
