`ifndef CNN_TOP_SV 
`define CNN_TOP_SV

module cnn_top;

    import uvm_pkg::*;          // import the UVM library
    `include "uvm_macros.svh"   // Include the UVM macros
    import test_pkg::*;

    logic clk;

    // interface
    cnn_interface cnn_vif(clk);

    cnn_ip_v1_0 DUT(
                .s00_axi_aclk                (clk),
                //AXI STREAM SLAVE
                .axis_s_data_in     (cnn_vif.axis_s_data_in),
                .axis_s_valid       (cnn_vif.axis_s_valid),
                .axis_s_last        (cnn_vif.axis_s_last),
                .axis_s_ready       (cnn_vif.axis_s_ready),
                .axis_s_tkeep        (cnn_vif.axis_s_tkeep),
                //AXI STREAM MASTER
                .axim_s_valid       (cnn_vif.axim_s_valid),
                .axim_s_last        (cnn_vif.axim_s_last),
                .axim_s_ready       (cnn_vif.axim_s_ready),
                .axim_s_data        (cnn_vif.axim_s_data),
                .axim_s_tkeep       (cnn_vif.axim_s_tkeep),
                //.input_command      (cnn_vif.input_command),
                
                //AXI LITE
                .s00_axi_awaddr     (cnn_vif.s00_axi_awaddr),
                .s00_axi_awprot     (cnn_vif.s00_axi_awprot),
                .s00_axi_awvalid    (cnn_vif.s00_axi_awvalid),
                .s00_axi_awready    (cnn_vif.s00_axi_awready),
                .s00_axi_wdata      (cnn_vif.s00_axi_wdata),
                .s00_axi_wstrb      (cnn_vif.s00_axi_wstrb),
                .s00_axi_wvalid     (cnn_vif.s00_axi_wvalid),
                .s00_axi_wready     (cnn_vif.s00_axi_wready),
                .s00_axi_bresp      (cnn_vif.s00_axi_bresp),
                .s00_axi_bvalid     (cnn_vif.s00_axi_bvalid),
                .s00_axi_bready     (cnn_vif.s00_axi_bready),
                .s00_axi_araddr     (cnn_vif.s00_axi_araddr),
                .s00_axi_arprot     (cnn_vif.s00_axi_arprot),
                .s00_axi_arvalid    (cnn_vif.s00_axi_arvalid),
                .s00_axi_arready    (cnn_vif.s00_axi_arready),
                .s00_axi_rdata      (cnn_vif.s00_axi_rdata),
                .s00_axi_rresp      (cnn_vif.s00_axi_rresp),
                .s00_axi_rvalid     (cnn_vif.s00_axi_rvalid),
                .s00_axi_rready     (cnn_vif.s00_axi_rready),
                .s00_axi_aresetn    (cnn_vif.s00_axi_aresetn),
                //INTERUPT
                .interupt_done    (cnn_vif.end_command_int)


                );

    // run test
    initial begin
        uvm_config_db#(virtual cnn_interface)::set(null, "uvm_test_top.env", "cnn_interface", cnn_vif);
        run_test();
    end

    // clock init
    initial begin
        clk = 1;        
    end

    //clock generation
    always #5 clk = ~clk;

    initial begin
    #100ms;
	$display("Sorry! Ran out of clock cycles!");
        $finish();
    end

endmodule : cnn_top

`endif 
