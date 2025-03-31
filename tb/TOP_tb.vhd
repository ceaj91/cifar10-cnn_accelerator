
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity TOP_tb is
generic (

        CONVOLUTION_TO_TEST : integer :=1;
        WEIGHTS_0_LAYER : integer := 864;
        WEIGHTS_1_LAYER : integer := 4608;
        WEIGHTS_2_LAYER : integer := 4608;
        
        PICTURE_0_LAYER : integer := 3468;
        PICTURE_1_LAYER : integer := 10368;
        PICTURE_2_LAYER : integer := 3200;
        
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
end TOP_tb;

architecture Behavioral of TOP_tb is

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

signal    clk_s:  std_logic; 
    --AXI_SLAVE_STREAM signals
signal    axis_s_data_in_s:  std_logic_vector(RAM_WIDTH-1 downto 0);
signal    axis_s_valid_s: std_logic;
signal    axis_s_last_s: std_logic;
signal    axis_s_ready_s: std_logic;
    
signal    input_command_s:  std_logic_vector(13 downto 0);
    
    --interrupt
signal    end_command_int_s:  std_logic;
  
  ----AXI_MASTER_STREAM  signals
signal    axim_s_valid_s: std_logic;
signal    axim_s_last_s: std_logic;
signal    axim_s_ready_s: std_logic;
signal    axim_s_data_s:  std_logic_vector(RAM_WIDTH-1 downto 0);

begin

duv: TOP_cnn
generic map(RAM_WIDTH => RAM_WIDTH,
        
            OUTPUT_PIC_RAM_DEPTH =>OUTPUT_PIC_RAM_DEPTH ,
            INPUT_PIC_RAM_DEPTH  => INPUT_PIC_RAM_DEPTH,
            INPUT_WEIGHT_RAM_DEPTH => INPUT_WEIGHT_RAM_DEPTH,
            INPUT_BIAS_DEPTH  => INPUT_BIAS_DEPTH,
            
            OUTPUT_PIC_ADDR_SIZE  => OUTPUT_PIC_ADDR_SIZE,
            INPUT_PIC_ADDR_SIZE =>INPUT_PIC_ADDR_SIZE , 
            INPUT_WEIGHT_ADDR_SIZE =>INPUT_WEIGHT_ADDR_SIZE ,
            INPUT_BIAS_ADDR_SIZE  =>INPUT_BIAS_ADDR_SIZE ,
           
            LINE_LENGHT_1  =>LINE_LENGHT_1 ,
            LINE_LENGHT_2  =>LINE_LENGHT_2 ,
            NUM_OF_LINES_IN_CACHE => NUM_OF_LINES_IN_CACHE)
port map(clk => clk_s, 
        --AXI_SLAVE_STREAM signals
        axis_s_data_in=>axis_s_data_in_s , 
        axis_s_valid=>axis_s_valid_s , 
        axis_s_last=> axis_s_last_s, 
        axis_s_ready=> axis_s_ready_s, 
        
        input_command=>input_command_s , 
        
        --interrupt
        end_command_int=> end_command_int_s, 
      
      ----AXI_MASTER_STREAM  signals
        axim_s_valid=> axim_s_valid_s, 
        axim_s_last=> axim_s_last_s, 
        axim_s_ready=> axim_s_ready_s, 
        axim_s_data=>axim_s_data_s
         );

        
clk_gen: process  begin

clk_s <= '0';
wait for 5 ns;
clk_s <= '1';
wait for 5 ns;
end process;  

stim_gen: process  

variable brojac : integer := 0;
variable data : integer := 0;
begin

axis_s_data_in_s <= (others => '0');
axis_s_valid_s <= '0';
axis_s_last_s <= '0';
input_command_s<="00010000000000"; -- restart
wait for 100 ns;
--send bias

input_command_s<="00000000000001";   --load bias
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

input_command_s<="00000000000000"; 
axis_s_valid_s <= '0';
axis_s_last_s<='0';
wait for 10 ns;
brojac :=0;
        
case (CONVOLUTION_TO_TEST) is
when 0 =>
        
        --send weights
        input_command_s<="00000000000010";
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
        input_command_s<="00000000000000";
        axis_s_valid_s <= '0';
        axis_s_last_s<='0';
        wait for 10 ns;
        brojac :=0;
        
        --send input picture
        input_command_s<="00000000000100";
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
        input_command_s<="00000000000000";
        axis_s_valid_s <= '0';
        axis_s_last_s<='0';
        wait for 10 ns;
        brojac :=0;
        input_command_s<="00000000001000";
        while(end_command_int_s = '0') loop
            wait for 10 ns;
        end loop;
        wait for 10 ns;
        input_command_s<="00100000000000";
        axim_s_ready_s<='1';
        while(end_command_int_s = '0') loop
            wait for 10 ns;
        end loop;
        input_command_s<="00000000000000";
        
when 1 =>

        --send input picture
        input_command_s<="00000000100000"; --send input picture 18*18*32 (padded image)
        while(brojac < PICTURE_1_LAYER) loop
            if(brojac = PICTURE_1_LAYER-1) then
                axis_s_last_s<='1';
            end if;
            if(axis_s_ready_s = '1') then
                axis_s_valid_s <= '1';
                axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
                brojac := brojac + 1;
            end if;
            wait for 10 ns;
        end loop;
        input_command_s<="00000000000000";
        axis_s_valid_s <= '0';
        axis_s_last_s<='0';
        wait for 10 ns;
        brojac :=0;
        data :=0;
        for i in 0 to 1 loop
                 --send weights
                input_command_s<="00000000010000"; --send weights 3x3x32x16
                while(brojac < WEIGHTS_1_LAYER) loop
                    if(brojac = WEIGHTS_1_LAYER-1) then
                        axis_s_last_s<='1';
                    end if;
                    if(axis_s_ready_s = '1') then
                        axis_s_valid_s <= '1';
                        axis_s_data_in_s <= std_logic_vector(to_unsigned(data,RAM_WIDTH));
                        brojac := brojac + 1;
                        data := data+1;
                    end if;
                    wait for 10 ns;
                end loop;
                input_command_s<="00000000000000";
                axis_s_valid_s <= '0';
                axis_s_last_s<='0';
                wait for 10 ns;
              --start convolution first time
                input_command_s<="00000001000000";
                while(end_command_int_s = '0') loop
                    wait for 10 ns;
                end loop;
                wait for 10 ns;
                
                input_command_s<="00000000000000";
                brojac :=0;
                wait for 30 ns;
        end loop;
         ----------------------------------------------
       ----------------------------------------------------------------  
         --read data now
        input_command_s<="01000000000000";
        axim_s_ready_s<='1';
        while(end_command_int_s = '0') loop
            wait for 10 ns;
        end loop;
        input_command_s<="00000000000000";
        wait for 10 ns;
        wait;
        
when others =>

        --send input picture
        input_command_s<="00000100000000"; --send input picture 10*10*32 (padded image)
        while(brojac < PICTURE_2_LAYER) loop
            if(brojac = PICTURE_2_LAYER-1) then
                axis_s_last_s<='1';
            end if;
            if(axis_s_ready_s = '1') then
                axis_s_valid_s <= '1';
                axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
                brojac := brojac + 1;
            end if;
            wait for 10 ns;
        end loop;
        input_command_s<="00000000000000";
        axis_s_valid_s <= '0';
        axis_s_last_s<='0';
        wait for 10 ns;
        brojac :=0;
        
        for i in 0 to 4 loop
                 --send weights
                input_command_s<="00000010000000"; --send weights 3x3x32x16
                while(brojac < WEIGHTS_2_LAYER) loop
                    if(brojac = WEIGHTS_2_LAYER-1) then
                        axis_s_last_s<='1';
                    end if;
                    if(axis_s_ready_s = '1') then
                        axis_s_valid_s <= '1';
                        axis_s_data_in_s <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
                        brojac := brojac + 1;
                    end if;
                    wait for 10 ns;
                end loop;
                input_command_s<="00000000000000";
                axis_s_valid_s <= '0';
                axis_s_last_s<='0';
                wait for 10 ns;
              --start convolution first time
                input_command_s<="00001000000000";
                while(end_command_int_s = '0') loop
                    wait for 10 ns;
                end loop;
                wait for 10 ns;
                
                input_command_s<="00000000000000";
                wait for 10 ns;
        end loop;
           
         --read data now
        input_command_s<="10000000000000";
        axim_s_ready_s<='1';
        while(end_command_int_s = '0') loop
            wait for 10 ns;
        end loop;
        input_command_s<="00000000000000";
        wait for 10 ns;
end case;



wait;
end process;

end Behavioral;
