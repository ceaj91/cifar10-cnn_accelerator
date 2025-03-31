`ifndef CNN_INTERFACE
`define CNN_INTERFACE

interface cnn_interface(input clk);


    parameter   C_S00_AXI_DATA_WIDTH = 32;
    parameter   C_S00_AXI_ADDR_WIDTH =  4;
    
    //AXI STREAM SLAVE 
    logic [15 : 0] axis_s_data_in;  //input
    logic axis_s_valid;             //input
    logic axis_s_ready;             //OUTPUT
    logic axis_s_last;              //input    
    logic [1:0]axis_s_tkeep;

    //AXI STREAM MASTER
    logic [15:0] axim_s_data;       //OUTPUT
    logic axim_s_valid;             //OUTPUT
    logic axim_s_ready;             //input
    logic axim_s_last;              //OUTPUT
    logic [1:0]axim_s_tkeep;
   
    logic end_command_int;          //OUTPUT

    //AXI LITE
    logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_awaddr;
    logic [2:0] s00_axi_awprot;
    logic s00_axi_awvalid;
    logic s00_axi_awready;
    logic [C_S00_AXI_DATA_WIDTH -1:0] s00_axi_wdata;
    logic [(C_S00_AXI_DATA_WIDTH/8) -1:0] s00_axi_wstrb;
    logic s00_axi_wvalid;
    logic s00_axi_wready;
    logic [1:0] s00_axi_bresp;
    logic s00_axi_bvalid;
    logic s00_axi_bready;
    logic [C_S00_AXI_ADDR_WIDTH -1:0] s00_axi_araddr;
    logic [2:0] s00_axi_arprot;
    logic s00_axi_arvalid;
    logic s00_axi_arready;
    logic [C_S00_AXI_DATA_WIDTH - 1:0] s00_axi_rdata;
    logic [1:0] s00_axi_rresp;
    logic s00_axi_rvalid;
    logic s00_axi_rready;
    logic s00_axi_aresetn;

endinterface: cnn_interface


`endif
