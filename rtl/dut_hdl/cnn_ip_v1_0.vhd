library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cnn_ip_v1_0 is
	generic (
		-- Users to add parameters here
        RAM_WIDTH : integer := 16;
        
        OUTPUT_PIC_RAM_DEPTH: integer := 32768;
        INPUT_PIC_RAM_DEPTH : integer := 16384;
        INPUT_WEIGHT_RAM_DEPTH : integer := 8192;
        INPUT_BIAS_DEPTH : integer := 128;
        
        OUTPUT_PIC_ADDR_SIZE : integer := 15;
        INPUT_PIC_ADDR_SIZE : integer := 14;
        INPUT_WEIGHT_ADDR_SIZE: integer :=13;
        INPUT_BIAS_ADDR_SIZE : integer :=7;
       
        LINE_LENGHT_1 : integer := 3;
        LINE_LENGHT_2 : integer := 32;
        NUM_OF_LINES_IN_CACHE : integer := 9;
        COMMAND_REG_WIDTH: integer := 14; 
		-- User parameters ends
		-- Do not modify the parameters beyond this line


		-- Parameters of Axi Slave Bus Interface S00_AXI
		C_S00_AXI_DATA_WIDTH	: integer	:= 32;
		C_S00_AXI_ADDR_WIDTH	: integer	:= 4
	);
	port (
		-- Users to add ports here
        interupt_done: out std_logic;
        --AXI STREAM SLAVE SIGNALS
         axis_s_data_in: in std_logic_vector(15 downto 0);
        axis_s_valid:in std_logic;
        axis_s_last:in std_logic;
        axis_s_ready:out std_logic;
        axis_s_tkeep:in std_logic_vector(1 downto 0);
        --AXI STREAM MASTER SIGNALS
        axim_s_valid:out std_logic;
        axim_s_last:out std_logic;
        axim_s_ready:in std_logic;
        axim_s_data: out std_logic_vector(15 downto 0);
        axim_s_tkeep:out std_logic_vector(1 downto 0);
		-- User ports ends
		-- Do not modify the ports beyond this line

        
		-- Ports of Axi Slave Bus Interface S00_AXI
		s00_axi_aclk	: in std_logic;
		s00_axi_aresetn	: in std_logic;
		s00_axi_awaddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_awprot	: in std_logic_vector(2 downto 0);
		s00_axi_awvalid	: in std_logic;
		s00_axi_awready	: out std_logic;
		s00_axi_wdata	: in std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_wstrb	: in std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
		s00_axi_wvalid	: in std_logic;
		s00_axi_wready	: out std_logic;
		s00_axi_bresp	: out std_logic_vector(1 downto 0);
		s00_axi_bvalid	: out std_logic;
		s00_axi_bready	: in std_logic;
		s00_axi_araddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_arprot	: in std_logic_vector(2 downto 0);
		s00_axi_arvalid	: in std_logic;
		s00_axi_arready	: out std_logic;
		s00_axi_rdata	: out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_rresp	: out std_logic_vector(1 downto 0);
		s00_axi_rvalid	: out std_logic;
		s00_axi_rready	: in std_logic
	);
end cnn_ip_v1_0;

architecture arch_imp of cnn_ip_v1_0 is

	-- component declaration
	component cnn_ip_v1_0_S00_AXI
		generic (
		-- Users to add parameters here
        COMMAND_REG_WIDTH: integer := 14;
		-- User parameters ends
		-- Do not modify the parameters beyond this line

		-- Width of S_AXI data bus
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		-- Width of S_AXI address bus
		C_S_AXI_ADDR_WIDTH	: integer	:= 4
	       );
		
		port (
		reset_ip_command_reg : in std_logic;
		ip_command : out std_logic_vector(COMMAND_REG_WIDTH-1 downto 0);
		 
		S_AXI_ACLK	: in std_logic;
		S_AXI_ARESETN	: in std_logic;
		S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
		S_AXI_AWVALID	: in std_logic;
		S_AXI_AWREADY	: out std_logic;
		S_AXI_WDATA	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_WSTRB	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		S_AXI_WVALID	: in std_logic;
		S_AXI_WREADY	: out std_logic;
		S_AXI_BRESP	: out std_logic_vector(1 downto 0);
		S_AXI_BVALID	: out std_logic;
		S_AXI_BREADY	: in std_logic;
		S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
		S_AXI_ARVALID	: in std_logic;
		S_AXI_ARREADY	: out std_logic;
		S_AXI_RDATA	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_RRESP	: out std_logic_vector(1 downto 0);
		S_AXI_RVALID	: out std_logic;
		S_AXI_RREADY	: in std_logic
		);
	end component;
component TOP_cnn
            generic (
                RAM_WIDTH : integer := 16;
                
                OUTPUT_PIC_RAM_DEPTH: integer := 32768;
                INPUT_PIC_RAM_DEPTH : integer := 16384;
                INPUT_WEIGHT_RAM_DEPTH : integer := 8192;
                INPUT_BIAS_DEPTH : integer := 128;
                
                OUTPUT_PIC_ADDR_SIZE : integer := 15;
                INPUT_PIC_ADDR_SIZE : integer := 14;
                INPUT_WEIGHT_ADDR_SIZE: integer :=13;
                INPUT_BIAS_ADDR_SIZE : integer :=7;
               
                LINE_LENGHT_1 : integer := 3;
                LINE_LENGHT_2 : integer := 32;
                NUM_OF_LINES_IN_CACHE : integer := 9
                
                );
            Port(clk: in std_logic; 
                --AXI_SLAVE_STREAM signals
                axis_s_data_in: in std_logic_vector(RAM_WIDTH-1 downto 0);
                axis_s_valid:in std_logic;
                axis_s_last:in std_logic;
                axis_s_ready:out std_logic;
                
                input_command: in std_logic_vector(13 downto 0);
                
                --interrupt
                end_command_int: out std_logic;
              
              ----AXI_MASTER_STREAM  signals
                axim_s_valid:out std_logic;
                axim_s_last:out std_logic;
                axim_s_ready:in std_logic;
                axim_s_data: out std_logic_vector(RAM_WIDTH-1 downto 0)
                 );
    
    end component;
    signal ip_command_s: std_logic_vector(COMMAND_REG_WIDTH-1 downto 0);
    signal int_s:std_logic;
begin

-- Instantiation of Axi Bus Interface S00_AXI
cnn_ip_v1_0_S00_AXI_inst : cnn_ip_v1_0_S00_AXI
	generic map (
	    COMMAND_REG_WIDTH   => COMMAND_REG_WIDTH,
		C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
		C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	)
	port map (
	    reset_ip_command_reg => int_s,
	    ip_command => ip_command_s,
		S_AXI_ACLK	=> s00_axi_aclk,
		S_AXI_ARESETN	=> s00_axi_aresetn,
		S_AXI_AWADDR	=> s00_axi_awaddr,
		S_AXI_AWPROT	=> s00_axi_awprot,
		S_AXI_AWVALID	=> s00_axi_awvalid,
		S_AXI_AWREADY	=> s00_axi_awready,
		S_AXI_WDATA	=> s00_axi_wdata,
		S_AXI_WSTRB	=> s00_axi_wstrb,
		S_AXI_WVALID	=> s00_axi_wvalid,
		S_AXI_WREADY	=> s00_axi_wready,
		S_AXI_BRESP	=> s00_axi_bresp,
		S_AXI_BVALID	=> s00_axi_bvalid,
		S_AXI_BREADY	=> s00_axi_bready,
		S_AXI_ARADDR	=> s00_axi_araddr,
		S_AXI_ARPROT	=> s00_axi_arprot,
		S_AXI_ARVALID	=> s00_axi_arvalid,
		S_AXI_ARREADY	=> s00_axi_arready,
		S_AXI_RDATA	=> s00_axi_rdata,
		S_AXI_RRESP	=> s00_axi_rresp,
		S_AXI_RVALID	=> s00_axi_rvalid,
		S_AXI_RREADY	=> s00_axi_rready
	);

	TOP_cnn_inst: TOP_cnn 
    generic map(RAM_WIDTH =>RAM_WIDTH ,
                    
                OUTPUT_PIC_RAM_DEPTH=>OUTPUT_PIC_RAM_DEPTH ,
                INPUT_PIC_RAM_DEPTH =>INPUT_PIC_RAM_DEPTH ,
                INPUT_WEIGHT_RAM_DEPTH =>INPUT_WEIGHT_RAM_DEPTH ,
                INPUT_BIAS_DEPTH => INPUT_BIAS_DEPTH,
                
                OUTPUT_PIC_ADDR_SIZE => OUTPUT_PIC_ADDR_SIZE,
                INPUT_PIC_ADDR_SIZE => INPUT_PIC_ADDR_SIZE,
                INPUT_WEIGHT_ADDR_SIZE=> INPUT_WEIGHT_ADDR_SIZE,
                INPUT_BIAS_ADDR_SIZE =>INPUT_BIAS_ADDR_SIZE ,
               
                LINE_LENGHT_1 =>LINE_LENGHT_1 ,
                LINE_LENGHT_2 => LINE_LENGHT_2,
                NUM_OF_LINES_IN_CACHE => NUM_OF_LINES_IN_CACHE)
    port map(clk=> s00_axi_aclk, 
                --AXI_SLAVE_STREAM signals
            axis_s_data_in => axis_s_data_in,
            axis_s_valid=>axis_s_valid ,
            axis_s_last=>axis_s_last ,
            axis_s_ready=>axis_s_ready ,
            
            input_command=>ip_command_s ,
            
            --interrupt
            end_command_int=> int_s,
          
          ----AXI_MASTER_STREAM  signals
            axim_s_valid=>axim_s_valid ,
            axim_s_last=>axim_s_last ,
            axim_s_ready=>axim_s_ready ,
            axim_s_data=> axim_s_data
             );
	-- Add user logic here
interupt_done<=int_s;
axim_s_tkeep<="11";
	-- User logic ends

end arch_imp;
