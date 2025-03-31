

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;


entity IP_tb is
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
end IP_tb;

architecture Behavioral of IP_tb is
component cnn_ip_v1_0
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
end component;

signal clk_s:std_logic;
signal interupt_done_s:std_logic;
signal axis_s_valid_s, axis_s_last_s, axis_s_ready_s, axim_s_valid_s, axim_s_last_s, axim_s_ready_s:std_logic;
signal axis_s_tkeep_s,axim_s_tkeep_s : std_logic_vector(1 downto 0);
signal axis_s_data_in_s,axim_s_data_s:std_logic_vector(15 downto 0);

signal s00_axi_aresetn_s,s00_axi_awvalid_s,s00_axi_awready_s,s00_axi_wvalid_s,s00_axi_wready_s,s00_axi_bvalid_s,s00_axi_bready_s,s00_axi_arvalid_s,s00_axi_arready_s,s00_axi_rvalid_s, s00_axi_rready_s:std_logic;
signal s00_axi_wdata_s,s00_axi_rdata_s: std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
signal s00_axi_awaddr_s,s00_axi_araddr_s:std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
signal s00_axi_awprot_s,s00_axi_arprot_s: std_logic_vector(2 downto 0);
signal s00_axi_bresp_s,s00_axi_rresp_s:std_logic_vector(1 downto 0);
signal s00_axi_wstrb_s: std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
begin 
duv: cnn_ip_v1_0
generic map(RAM_WIDTH => RAM_WIDTH,
        
            OUTPUT_PIC_RAM_DEPTH=>OUTPUT_PIC_RAM_DEPTH ,
            INPUT_PIC_RAM_DEPTH =>INPUT_PIC_RAM_DEPTH ,
            INPUT_WEIGHT_RAM_DEPTH =>INPUT_WEIGHT_RAM_DEPTH ,
            INPUT_BIAS_DEPTH => INPUT_BIAS_DEPTH,
            
            OUTPUT_PIC_ADDR_SIZE => OUTPUT_PIC_ADDR_SIZE,
            INPUT_PIC_ADDR_SIZE => INPUT_PIC_ADDR_SIZE,
            INPUT_WEIGHT_ADDR_SIZE=> INPUT_WEIGHT_ADDR_SIZE,
            INPUT_BIAS_ADDR_SIZE=> INPUT_BIAS_ADDR_SIZE,
           
            LINE_LENGHT_1 => LINE_LENGHT_1,
            LINE_LENGHT_2 => LINE_LENGHT_2,
            NUM_OF_LINES_IN_CACHE => NUM_OF_LINES_IN_CACHE,
            COMMAND_REG_WIDTH=> COMMAND_REG_WIDTH, 
            -- User parameters ends
            -- Do not modify the parameters beyond this line
    
    
            -- Parameters of Axi Slave Bus Interface S00_AXI
            C_S00_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
            C_S00_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH)
port map(
        interupt_done=>interupt_done_s,
        --AXI STREAM SLAVE SIGNALS
        axis_s_data_in => axis_s_data_in_s,
        axis_s_valid => axis_s_valid_s,
        axis_s_last=> axis_s_last_s,
        axis_s_ready=> axis_s_ready_s,
        axis_s_tkeep=> axis_s_tkeep_s,
        --AXI STREAM MASTER SIGNALS
        axim_s_valid=> axim_s_valid_s,
        axim_s_last=> axim_s_last_s,
        axim_s_ready=> axim_s_ready_s,
        axim_s_data=> axim_s_data_s,
        axim_s_tkeep=> axim_s_tkeep_s,
        -- User ports ends
        -- Do not modify the ports beyond this line

        
        -- Ports of Axi Slave Bus Interface S00_AXI
        s00_axi_aclk	=> clk_s,
        s00_axi_aresetn	=> s00_axi_aresetn_s,
        s00_axi_awaddr	=> s00_axi_awaddr_s,
        s00_axi_awprot	=> s00_axi_awprot_s,
        s00_axi_awvalid	=> s00_axi_awvalid_s,
        s00_axi_awready	=> s00_axi_awready_s,
        s00_axi_wdata	=> s00_axi_wdata_s,
        s00_axi_wstrb	=> s00_axi_wstrb_s,
        s00_axi_wvalid	=> s00_axi_wvalid_s,
        s00_axi_wready	=> s00_axi_wready_s,
        s00_axi_bresp	=> s00_axi_bresp_s,
        s00_axi_bvalid	=> s00_axi_bvalid_s,
        s00_axi_bready	=> s00_axi_bready_s,
        s00_axi_araddr	=> s00_axi_araddr_s,
        s00_axi_arprot	=> s00_axi_arprot_s,
        s00_axi_arvalid	=> s00_axi_arvalid_s ,
        s00_axi_arready	=> s00_axi_arready_s,
        s00_axi_rdata	=> s00_axi_rdata_s,
        s00_axi_rresp	=> s00_axi_rresp_s,
        s00_axi_rvalid	=> s00_axi_rvalid_s,
        s00_axi_rready	=> s00_axi_rready_s);

clk_gen: process is begin
clk_s <= '0';
wait for 5ns;
clk_s <= '1';
wait for 5ns;
end process;  



stim_gen: process is 
variable brojac : integer := 0;
variable data : integer := 0;

begin 

    --axi lite RESET COMMAND
    wait for 10 ns;
    
    -- AXI Lite Write Command
    s00_axi_aresetn_s <= '0'; -- Assert reset
    wait for 50 ns;
    s00_axi_aresetn_s <= '1'; -- Deassert reset

    -- Wait for a few clock cycles to stabilize after reset
    wait for 50 ns;

    -- Send AXI Lite write command to write some data to a specific address
    --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000400";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    wait for 50ns;

     s00_axi_aresetn_s <= '0'; -- Assert reset
    wait for 50 ns;
    s00_axi_aresetn_s <= '1'; -- Deassert reset

    -- Wait for a few clock cycles to stabilize after reset
    wait for 50 ns;

    -- Send AXI Lite write command to write some data to a specific address
    --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000001";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
axis_s_last_s<='0';
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

brojac :=0;
while(brojac < 128) loop
    if(brojac = 127) then
        axis_s_last_s<='1';
    end if;
    if(axis_s_ready_s = '1') then
        axis_s_valid_s <= '1';
        axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
        brojac := brojac + 1;
    end if;
    wait for 10 ns;
end loop;
axis_s_last_s<='0';
 axis_s_valid_s <= '0';

 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   
    -- Wait for a few clock cycles to stabilize after reset
    wait for 50 ns;

    -- Send AXI Lite write command to write some data to a specific address
    --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000002";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  brojac :=0;
   while(brojac < 864) loop
            if(brojac = 863) then
                axis_s_last_s<='1';
            end if;
            if(axis_s_ready_s = '1') then
                axis_s_valid_s <= '1';
                axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
                brojac := brojac + 1;
            end if;
            wait for 10 ns;
        end loop;
 axis_s_last_s<='0';
 axis_s_valid_s <= '0';
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   
    -- Wait for a few clock cycles to stabilize after reset
    wait for 50 ns;

    -- Send AXI Lite write command to write some data to a specific address
    --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000004";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  brojac :=0;
  while(brojac < 3468) loop
            if(brojac = 3467) then
                axis_s_last_s<='1';
            end if;
            if(axis_s_ready_s = '1') then
                axis_s_valid_s <= '1';
                axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
                brojac := brojac + 1;
            end if;
            wait for 10 ns;
        end loop;
 axis_s_last_s<='0';
 axis_s_valid_s <= '0';       
 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     --wait for 50 ns;

    -- Send AXI Lite write command to write some data to a specific address
    --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000008";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
    
    while( interupt_done_s = '0') loop
            wait for 10 ns;
    end loop;
-----------------------------
      --adress signals
    s00_axi_awvalid_s <= '1';
    s00_axi_wvalid_s<='1';
    s00_axi_awaddr_s <= "0000"; -- Replace "0000" with the address where you want to write the data
    while( s00_axi_awready_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_wdata_s <= x"00000800";
    s00_axi_wstrb_s <= "1111"; -- Set the byte enables to indicate all bytes are valid (writing 32-bit data)
 
    wait for 10 ns;
    s00_axi_wvalid_s<='0';
    

    while( s00_axi_bvalid_s = '0') loop
            wait for 10 ns;
    end loop;
    s00_axi_bready_s<='1';
    wait for 10 ns;
    s00_axi_bready_s<='0';
    
    axim_s_ready_s<='1';
 -----------------------------
   
    wait;
    -- You can add more write commands or read commands as needed

    -- Stimulus is complete

end process;

end Behavioral;
