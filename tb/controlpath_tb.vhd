

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;


entity controlpath_tb is
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
--  Port ( );
end controlpath_tb;

architecture Behavioral of controlpath_tb is
signal clk : std_logic;
--AXI_STREAM SLAVE signals
signal    axis_s_data_in:  std_logic_vector(RAM_WIDTH-1 downto 0);
signal    axis_s_valid: std_logic;
signal    axis_s_last: std_logic;
signal    axis_s_ready: std_logic;
    --input map memmory control
signal     en_inmem_pic:  std_logic;
signal     we_inmem_pic:  std_logic;
signal     addr_read1_inmem_pic :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_read2_inmem_pic :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_read3_inmem_pic :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_write_inmem_pic :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     data_in_inmem_pic :  std_logic_vector(RAM_WIDTH - 1 downto 0); 
     --input weights
signal     en_inmem_weights:  std_logic;
signal     we_inmem_weights:  std_logic;
signal     addr_read1_inmem_weights :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     addr_read2_inmem_weights :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     addr_read3_inmem_weights :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
 signal    addr_write_inmem_weights :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
 signal    data_in_inmem_weights :  std_logic_vector(RAM_WIDTH - 1 downto 0);
      --input bias
 signal   en_biasmem:  std_logic;
 signal   we_biasmem:  std_logic;
 signal   addr_read_biasmem :  std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
 signal   addr_write_biasmem:  std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
  signal  data_inbias :  std_logic_vector(RAM_WIDTH - 1 downto 0);
     --output map memory control
  signal  en_outmem:  std_logic;
  signal addr_read_outmem :  std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);
  signal reset_out_mem_counter:  std_logic;

      --cache block pic 1
 signal en_cacheblck_pic1 :  std_logic;
 signal reset_cachelck_pic1:  std_logic;
  
  --cache block pic 2 and 3
  signal en_cacheblck_pic23 :  std_logic;
  signal reset_cachelck_pic23:  std_logic;
  
  --cache block weights 1
  signal en1_cacheblck_weights1 :  std_logic;
  signal en2_cacheblck_weights1 :  std_logic;
  signal en3_cacheblck_weights1 :  std_logic;
  signal mode_cacheblck_weights1: std_logic;
  signal reset_cachelck_weights1:  std_logic;
  
  --cache block weights 2 and 3 
 signal  en1_cacheblck_weights23 :  std_logic;
 signal  en2_cacheblck_weights23 :  std_logic;
 signal  en3_cacheblck_weights23 :  std_logic;
 signal mode_cacheblck_weights23 : std_logic;
 signal reset_cachelck_weights23:  std_logic;
  
  --MACs modules
signal  reset_mac:  std_logic;
 signal en_mac :  std_logic;
  
  --add tree 
 signal en_add :  std_logic;
 signal reset_add:  std_logic;

  
  --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
 signal mode_choice :  std_logic;
   
 --command_reg input
 signal input_command :  std_logic_vector(13 downto 0);
 --interrupt
  signal end_command_int:  std_logic;
 --how many data to send from output memory
 signal number_of_output_data:  std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1 downto 0);

  --AXI_MASTER_STREAM  signals
  signal axim_s_valid: std_logic;
  signal axim_s_last: std_logic;
  signal axim_s_ready: std_logic;
  
  begin

duv : entity work.controlpath
generic map(
        RAM_WIDTH =>RAM_WIDTH ,
        
        OUTPUT_PIC_RAM_DEPTH=>OUTPUT_PIC_RAM_DEPTH ,
        INPUT_PIC_RAM_DEPTH =>INPUT_PIC_RAM_DEPTH ,
        INPUT_WEIGHT_RAM_DEPTH =>INPUT_WEIGHT_RAM_DEPTH ,
        INPUT_BIAS_DEPTH =>INPUT_BIAS_DEPTH ,
        
        OUTPUT_PIC_ADDR_SIZE =>OUTPUT_PIC_ADDR_SIZE ,
        INPUT_PIC_ADDR_SIZE => INPUT_PIC_ADDR_SIZE,
        INPUT_WEIGHT_ADDR_SIZE=> INPUT_WEIGHT_ADDR_SIZE,
        INPUT_BIAS_ADDR_SIZE => INPUT_BIAS_ADDR_SIZE,
       
        LINE_LENGHT_1 => LINE_LENGHT_1,
        LINE_LENGHT_2 => LINE_LENGHT_2,
        NUM_OF_LINES_IN_CACHE => NUM_OF_LINES_IN_CACHE)
port map( 
        clk => clk,
        --AXI_STREAM SLAVE signals
        axis_s_data_in=>axis_s_data_in ,
        axis_s_valid=> axis_s_valid,
        axis_s_last=>axis_s_last ,
        axis_s_ready=> axis_s_ready,
        --input map memmory control
        en_inmem_pic=>en_inmem_pic ,
        we_inmem_pic=>we_inmem_pic ,
        addr_read1_inmem_pic => addr_read1_inmem_pic,
        addr_read2_inmem_pic => addr_read2_inmem_pic,
        addr_read3_inmem_pic => addr_read3_inmem_pic,
        addr_write_inmem_pic => addr_write_inmem_pic,
        data_in_inmem_pic =>data_in_inmem_pic ,
        --input weights
        en_inmem_weights=> en_inmem_weights,
        we_inmem_weights=>we_inmem_weights ,
        addr_read1_inmem_weights=>addr_read1_inmem_weights ,
        addr_read2_inmem_weights =>addr_read2_inmem_weights ,
        addr_read3_inmem_weights =>addr_read3_inmem_weights ,
        addr_write_inmem_weights =>addr_write_inmem_weights ,
        data_in_inmem_weights =>data_in_inmem_weights ,
        --input bias
        en_biasmem=>en_biasmem ,
        we_biasmem=> we_biasmem,
        addr_read_biasmem=> addr_read_biasmem,
        addr_write_biasmem=>addr_write_biasmem ,
        data_inbias =>data_inbias ,
        --output map memory control
        en_outmem=> en_outmem,
        addr_read_outmem =>addr_read_outmem ,
        reset_out_mem_counter=>reset_out_mem_counter,
        --cache block pic 1
        en_cacheblck_pic1 =>en_cacheblck_pic1 ,
        reset_cachelck_pic1=> reset_cachelck_pic1,
        
        --cache block pic 2 and 3
        en_cacheblck_pic23 =>en_cacheblck_pic23 ,
        reset_cachelck_pic23=> reset_cachelck_pic23,
        
        --cache block weights 1
        en1_cacheblck_weights1 =>en1_cacheblck_weights1 ,
        en2_cacheblck_weights1 =>en2_cacheblck_weights1 ,
        en3_cacheblck_weights1 =>en3_cacheblck_weights1 ,
        mode_cacheblck_weights1 => mode_cacheblck_weights1,
        reset_cachelck_weights1=>reset_cachelck_weights1 ,
        
        --cache block weights 2 and 3 
        en1_cacheblck_weights23 =>en1_cacheblck_weights23 ,
        en2_cacheblck_weights23 =>en2_cacheblck_weights23 ,
        en3_cacheblck_weights23 =>en3_cacheblck_weights23 ,
        mode_cacheblck_weights23 => mode_cacheblck_weights23,
        reset_cachelck_weights23=> reset_cachelck_weights23,
        
        --MACs modules
        reset_mac=> reset_mac,
        en_mac => en_mac,
        
        --add tree 
        en_add => en_add,
        reset_add=>reset_add ,

        --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
        mode_choice =>mode_choice,
        
        --input command from axi lite
        input_command=> input_command,
        
        --interupt
        end_command_int=>end_command_int,
        
        --number of data to read, from axi lite
        
        --AXI_MASTER_STREAM  signals
        axim_s_valid=>axim_s_valid,
        axim_s_last=>axim_s_last,
        axim_s_ready=>axim_s_ready
        );
        
        
clk_gen: process is begin

clk <= '0';
wait for 5ns;
clk <= '1';
wait for 5ns;
end process;  

stim_gen: process is 

variable brojac : integer := 0;

begin

axis_s_data_in <= (others => '0');
axis_s_valid <= '0';
axis_s_last <= '0';
input_command<="00010000000000";
wait for 100ns;
--send bias

input_command<="00000000000001";
while(brojac < 128) loop
    if(brojac = 127) then
        axis_s_last<='1';
    end if;
    if(axis_s_ready = '1') then
        axis_s_valid <= '1';
        axis_s_data_in <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
        brojac := brojac + 1;
    end if;
    wait for 10ns;
end loop;

input_command<="00000000000000";
axis_s_valid <= '0';
axis_s_last<='0';
wait for 10ns;
brojac :=0;

--send weights
input_command<="00000000000010";
while(brojac < 864) loop
    if(brojac = 863) then
        axis_s_last<='1';
    end if;
    if(axis_s_ready = '1') then
        axis_s_valid <= '1';
        axis_s_data_in <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
        brojac := brojac + 1;
    end if;
    wait for 10ns;
end loop;
input_command<="00000000000000";
axis_s_valid <= '0';
axis_s_last<='0';
wait for 10ns;
brojac :=0;

--send input picture
input_command<="00000000000100";
while(brojac < 3468) loop
    if(brojac = 3467) then
        axis_s_last<='1';
    end if;
    if(axis_s_ready = '1') then
        axis_s_valid <= '1';
        axis_s_data_in <= std_logic_vector(to_unsigned(brojac,RAM_WIDTH));
        brojac := brojac + 1;
    end if;
    wait for 10ns;
end loop;
input_command<="00000000000000";
axis_s_valid <= '0';
axis_s_last<='0';
wait for 10ns;
brojac :=0;
input_command<="00000000001000";
while(end_command_int = '0') loop
    wait for 10ns;
end loop;
wait for 10ns;
number_of_output_data <= std_logic_vector(to_unsigned(32767,15));
input_command<="00100000000000";
axim_s_ready<='1';
while(end_command_int = '0') loop
    wait for 10ns;
end loop;
input_command<="00000000000000";
wait;
end process;

      
end Behavioral;
 