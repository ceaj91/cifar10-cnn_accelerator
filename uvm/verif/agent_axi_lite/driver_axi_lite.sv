`ifndef DRIVER_AXI_LITE_SV
`define DRIVER_AXI_LITE_SV

class driver_axi_lite extends uvm_driver#(seq_item_lite);

    `uvm_component_utils(driver_axi_lite)

    seq_item_lite item;
    virtual interface cnn_interface vif;

    function new(string name = "driver", uvm_component parent = null);
        super.new(name,parent);
        
        if (!uvm_config_db#(virtual cnn_interface)::get(this, "", "cnn_interface", vif))
            `uvm_fatal("NOVIF",{"virtual interface must be set for: ",get_full_name(),".vif"})
            
        item = seq_item_lite::type_id::create("item");
    endfunction

    function void connect_phase(uvm_phase phase);
        super.connect_phase(phase);
    endfunction : connect_phase

    task main_phase(uvm_phase phase);

        forever begin
            @(posedge vif.clk);
            
            seq_item_port.get_next_item(item);
                //`uvm_info(get_name(), $sformatf("DRAJVER AXI LITE PRIMIO TRANSAKCIJU, SLEDECE PROTOKOL"), UVM_HIGH)
                 
                //postovanje protokola

                //resetuj svaki put
                vif.s00_axi_aresetn <= 1'b0;
                for(int i =0;i<2;i++) begin
                    @(posedge vif.clk);
                end

                vif.s00_axi_aresetn = 1'b1;

             
            vif.s00_axi_awaddr = 4'b0000;
            vif.s00_axi_wdata = item.COMMAND;
            vif.s00_axi_wstrb = 4'b1111;
            vif.s00_axi_awvalid = 1'b1;
            vif.s00_axi_wvalid = 1'b1;
            vif.s00_axi_bready = 1'b1;
            
            @(posedge vif.clk iff vif.s00_axi_awready);
            @(posedge vif.clk iff !vif.s00_axi_awready);
            #20
            vif.s00_axi_awvalid = 1'b0;
            vif.s00_axi_awaddr = 1'b0;
            vif.s00_axi_wdata = 1'b0;
            vif.s00_axi_wvalid = 1'b0;
            vif.s00_axi_wstrb = 4'b0000;
            
            @(posedge vif.clk iff !vif.s00_axi_bvalid);
            #20
            vif.s00_axi_bready = 1'b0;
            //$display("Data sent to AXI Lite registers");

                if(item.COMMAND == 32'h00000008  || item.COMMAND == 32'h00000040 || item.COMMAND == 32'h00000200)begin
                    @(posedge vif.clk iff vif.end_command_int);
                end
                else if(item.COMMAND == 32'h00000400) begin //if it is reset, wait 2 clock cycles
                    for(integer i=0; i<2; i++) begin
                        @(posedge vif.clk);
                    end
                    
                end
                    

            seq_item_port.item_done();
        end
    endtask : main_phase


endclass : driver_axi_lite

`endif
