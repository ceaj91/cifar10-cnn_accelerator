`ifndef TEST_XCELIUM_SV
`define TEST_XCELIUM_SV

class test_xcelium extends test_base;

    `uvm_component_utils(test_xcelium)

    simple_seq_lite lite_seq; 
    simple_seq_slave slave_seq; 
    simple_seq_master master_seq;

    function new(string name = "test_xcelium", uvm_component parent = null);
        super.new(name,parent);
    endfunction : new

    function void build_phase(uvm_phase phase);
        super.build_phase(phase);
        lite_seq = simple_seq_lite::type_id::create("lite_seq");
        slave_seq = simple_seq_slave::type_id::create("slave_seq");
        master_seq = simple_seq_master::type_id::create("master_seq");
    endfunction : build_phase

    task main_phase(uvm_phase phase);
        phase.raise_objection(this);

        lite_seq.command_global = 32'h00000400;                     //CMD-RESET IP
        lite_seq.start(env.agent_lite.seqr);
        lite_seq.command_global = 32'h00000001;                     //CMD-LOAD BIAS
        lite_seq.start(env.agent_lite.seqr);                        //ovaj test samo salje preko axi_lite nesto
        
        slave_seq.sending_bias = 1;                                 //LOAD BIAS
        slave_seq.start(env.agent_slave.seqr);
        
		`uvm_info(get_name(), $sformatf("================== START TEST_1 =================="),UVM_LOW)
        for(int i=0; i<10;i++) begin
                lite_seq.command_global = 32'h00000400;             //CMD-RESET IP
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00000002;             //CMD-LOAD WEIGHTS0
                lite_seq.start(env.agent_lite.seqr);
                
                slave_seq.sending_weigths0 = 1;                     //LOAD WEIGHTS0
                slave_seq.start(env.agent_slave.seqr);
                
                lite_seq.command_global = 32'h00000004;             //CMD-LOAD IMAGE0
                lite_seq.start(env.agent_lite.seqr);
                
                slave_seq.image_number_0 = 3468*i;
                slave_seq.sending_image0 = 1;                       //LOAD IMAGE0
                slave_seq.start(env.agent_slave.seqr);
                
                lite_seq.command_global = 32'h00000008;             //CMD-DO_CONV0
                lite_seq.start(env.agent_lite.seqr);
                
                lite_seq.command_global = 32'h00000800;             //CMD-READ DATA AFTER CONV0
                lite_seq.start(env.agent_lite.seqr);
                
                master_seq.param_to_send = 32768;
                master_seq.start(env.agent_master.seqr);
        end
        `uvm_info(get_name(), $sformatf("================== FINISHED TEST_1 =================="),UVM_LOW)
	`uvm_info(get_name(), $sformatf(" "),UVM_LOW)

		//======================================================================================================

		`uvm_info(get_name(), $sformatf("================== START TEST_2 =================="),UVM_LOW)
        for(int i = 0; i<10; i++) begin
                lite_seq.command_global = 32'h00000400;             //CMD-RESET IP
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00000010;             //CMD-LOAD WEIGHTS1_1
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto

                slave_seq.sending_weights1_1 = 1;                   //LOAD WEIGHTS1_1
                slave_seq.start(env.agent_slave.seqr);
        
                lite_seq.command_global = 32'h00000020;             //CMD-LOAD IMAGE1
                lite_seq.start(env.agent_lite.seqr);

                slave_seq.image_number_1 = 10368*i;
                slave_seq.sending_image1 = 1;                       //LOAD IMAGE1
                slave_seq.start(env.agent_slave.seqr);

                lite_seq.command_global = 32'h00000040;             //CMD-DO_CONV1
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00000010;             //CMD-LOAD WEIGHTS1_2
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto

                slave_seq.sending_weights1_2 = 1;                   //LOAD WEIGHTS1_2
                slave_seq.start(env.agent_slave.seqr);

                lite_seq.command_global = 32'h00000040;             //CMD-DO_CONV1
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00001000;             //CMD-READ DATA AFTER CONV1
                lite_seq.start(env.agent_lite.seqr);
                
                master_seq.param_to_send = 8192;
                master_seq.start(env.agent_master.seqr);
        end
        `uvm_info(get_name(), $sformatf("================== FINISHED TEST_2 =================="),UVM_LOW)
	`uvm_info(get_name(), $sformatf(" "),UVM_LOW)

		//======================================================================================================

		`uvm_info(get_name(), $sformatf("================== START TEST_3 =================="),UVM_LOW)
        for(int i = 0; i<10; i++) begin
                lite_seq.command_global = 32'h00000400;             //CMD-RESET IP
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00000080;             //CMD-LOAD WEIGHTS2_1
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto
                
                slave_seq.sending_weights2_1 = 1;                   //LOAD WEIGHTS2_1
                slave_seq.start(env.agent_slave.seqr);              
            
                lite_seq.command_global = 32'h00000100;             //CMD-LOAD IMAGE2
                lite_seq.start(env.agent_lite.seqr);
                
                slave_seq.image_number_2 = 3200*i;
                slave_seq.sending_image2 = 1;                       //LOAD IMAGE2
                slave_seq.start(env.agent_slave.seqr);
            
                lite_seq.command_global = 32'h00000200;             //CMD-DO_CONV2
                lite_seq.start(env.agent_lite.seqr);
                
                lite_seq.command_global = 32'h00000080;             //CMD-LOAD WEIGHTS2_2
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto

                slave_seq.sending_weights2_2 = 1;                   //LOAD WEIGHTS2_2
                slave_seq.start(env.agent_slave.seqr);              

                lite_seq.command_global = 32'h00000200;             //CMD-DO_CONV2
                lite_seq.start(env.agent_lite.seqr);
                
                lite_seq.command_global = 32'h00000080;             //CMD-LOAD WEIGHTS2_3
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto
                
                slave_seq.sending_weights2_3 = 1;                   //LOAD WEIGHTS2_3
                slave_seq.start(env.agent_slave.seqr);              

                lite_seq.command_global = 32'h00000200;             //CMD-DO_CONV2
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00000080;             //CMD-LOAD WEIGHTS2_4
                lite_seq.start(env.agent_lite.seqr);                //ovaj test samo salje preko axi_lite nesto
                
                slave_seq.sending_weights2_4 = 1;                   //LOAD WEIGHTS2_4
                slave_seq.start(env.agent_slave.seqr);              

                lite_seq.command_global = 32'h00000200;             //CMD-DO_CONV2
                lite_seq.start(env.agent_lite.seqr);

                lite_seq.command_global = 32'h00002000;             //CMD-READ DATA AFTER CONV2
                lite_seq.start(env.agent_lite.seqr);                
                
                master_seq.param_to_send = 4096;
                master_seq.start(env.agent_master.seqr);
    	end
    	`uvm_info(get_name(), $sformatf("================== FINISHED TEST_3 =================="),UVM_LOW)
	`uvm_info(get_name(), $sformatf(" "),UVM_LOW)
        

        #100ms;
        phase.drop_objection(this);
    endtask : main_phase

endclass

`endif

//#define IDLE_CMD 0x00000000
//#define LOAD_BIAS_CMD 0x00000001
//#define LOAD_WEIGHTS_0_CMD 0x00000002
//#define LOAD_PICTURE_0_CMD 0x00000004
//#define DO_CONV_0_CMD 0x00000008
//#define LOAD_WEIGHTS_1_CMD 0x00000010
//#define LOAD_PICTURE_1_CMD 0x00000020
//#define DO_CONV_1_CMD 0x00000040 
//#define LOAD_WEIGHTS_2_CMD 0x00000080
//#define LOAD_PICTURE_2_CMD 0x00000100
//#define DO_CONV_2_CMD 0x00000200
//#define RESET_CMD 0x00000400
//#define SEND_OUTPUT_FROM_CONV_0_CMD 0x00000800
//#define SEND_OUTPUT_FROM_CONV_1_CMD 0x00001000
//#define SEND_OUTPUT_FROM_CONV_2_CMD 0x00002000
