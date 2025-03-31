

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity datapath_cnn is
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
        
        ); --based on RAM_DEPTH   ADDR_SIZE = log2(RAM_DEPTH);

Port(clk:in std_logic;
--input map memmory control
     en_inmem_pic: in std_logic;
     we_inmem_pic: in std_logic;
     addr_read1_inmem_pic : in std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_read2_inmem_pic : in std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_read3_inmem_pic : in std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_write_inmem_pic : in std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     data_in_inmem_pic : in std_logic_vector(RAM_WIDTH - 1 downto 0);
     
 --input weights 
        
     en_inmem_weights: in std_logic;
     we_inmem_weights: in std_logic;
     addr_read1_inmem_weights : in std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_read2_inmem_weights : in std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_read3_inmem_weights : in std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_write_inmem_weights : in std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     data_in_inmem_weights : in std_logic_vector(RAM_WIDTH - 1 downto 0);
 
 --input bias
    en_biasmem: in std_logic;
    we_biasmem: in std_logic;
    addr_read_biasmem : in std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
    addr_write_biasmem: in std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
    data_inbias : in std_logic_vector(RAM_WIDTH - 1 downto 0);
 
 --output map memory control
    en_outmem: in std_logic;
    addr_read_outmem : in std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);
    data_outmem : out std_logic_vector(RAM_WIDTH - 1 downto 0);
    reset_out_mem_counter: in std_logic;
  --cache block pic 1
  en_cacheblck_pic1 : in std_logic;
  reset_cachelck_pic1: in std_logic;
  
  --cache block pic 2 and 3
  en_cacheblck_pic23 : in std_logic;
  reset_cachelck_pic23: in std_logic;
  
  --cache block weights 1
  en1_cacheblck_weights1 : in std_logic;
  en2_cacheblck_weights1 : in std_logic;
  en3_cacheblck_weights1 : in std_logic;
  mode_cacheblck_weights1 : in std_logic;
  reset_cachelck_weights1: in std_logic;

  --cache block weights 2 and 3 
  en1_cacheblck_weights23 : in std_logic;
  en2_cacheblck_weights23 : in std_logic;
  en3_cacheblck_weights23 : in std_logic;
  mode_cacheblck_weights23 : in std_logic;
  reset_cachelck_weights23: in std_logic;
  
  --MACs modules
  reset_mac: in std_logic;
  en_mac : in std_logic;
  
  --add tree 
  en_add : in std_logic;
  reset_add: in std_logic;
  
  --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
  mode_choice : in std_logic
     );
end datapath_cnn;

architecture Behavioral of datapath_cnn is

component BRAM_in_pic  
generic (
        RAM_WIDTH : integer := 16;
        RAM_DEPTH : integer := 16384;
        ADDR_SIZE : integer := 14); --based on RAM_DEPTH   ADDR_SIZE = log2(RAM_DEPTH);
port (
    clk : in std_logic;
    en : in std_logic;
    we : in std_logic;
    addr_read1 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_read2 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_read3 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_write : in std_logic_vector(ADDR_SIZE-1  downto 0);
    data_in : in std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out1 : out std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out2 : out std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out3 : out std_logic_vector(RAM_WIDTH - 1 downto 0)
  );
end component;

component BRAM_out_pic 
generic (                           
      RAM_WIDTH : integer := 16;    
      RAM_DEPTH : integer := 32768;
      ADDR_SIZE : integer := 15);--based on RAM_DEPTH   ADDR_SIZE = log2(RAM_DEPTH);
Port (  clk : in std_logic;                                     
 en : in std_logic;                                      
 we : in std_logic;                                      
 addr_read : in std_logic_vector(ADDR_SIZE-1  downto 0);                  
 addr_write : in std_logic_vector(ADDR_SIZE-1  downto 0);         
 data_in : in std_logic_vector(RAM_WIDTH - 1 downto 0);  
 data_out : out std_logic_vector(RAM_WIDTH - 1 downto 0));
end component;

component cache_block_picture 
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 32;
        NUM_OF_LINES: integer := 9);
Port (clk:in std_logic;
      reset:in std_logic;
      en: in std_logic;
      data_in1:in std_logic_vector(WORD_SIZE-1 downto 0);
      data_in2:in std_logic_vector(WORD_SIZE-1 downto 0);
      data_in3:in std_logic_vector(WORD_SIZE-1 downto 0);
      
      data_out1:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out2:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out3:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out4:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out5:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out6:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out7:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out8:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out9:out std_logic_vector(WORD_SIZE-1 downto 0));
end component;

component cache_block_weights 
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 3;
        NUM_OF_LINES: integer := 9);
Port (clk:in std_logic;
      reset:in std_logic;
      en1: in std_logic;
      en2: in std_logic;
      en3: in std_logic;
      mode: in std_logic;   -- 0-initial filling buffer      1 - circular buffer
      data_in1:in std_logic_vector(WORD_SIZE-1 downto 0);
      data_in2:in std_logic_vector(WORD_SIZE-1 downto 0);
      data_in3:in std_logic_vector(WORD_SIZE-1 downto 0);
      
      data_out1:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out2:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out3:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out4:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out5:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out6:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out7:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out8:out std_logic_vector(WORD_SIZE-1 downto 0);
      data_out9:out std_logic_vector(WORD_SIZE-1 downto 0));
end component;

component MAC_top 
generic (
        NUM_MACs : integer := 9;
        WORD_SIZE: integer := 16);
Port (    picture_params : in std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
          weights_params : in std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
          clk : in std_logic;
          reset : in std_logic;
          en : in std_logic;
          outputs : out std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0));  
end component;

component add_tree
generic(NUM_TO_ADD:integer := 9;
        WORD_SIZE: integer := 16);
Port (inputs_to_add : in std_logic_vector(NUM_TO_ADD * WORD_SIZE - 1 downto 0);
      bias : in std_logic_vector(WORD_SIZE-1 downto 0);
      add_enable : in std_logic;
      clk : in std_logic;
      reset : in std_logic;
      
      --out_valid: out std_logic;
      output_pixel : out std_logic_vector(WORD_SIZE - 1 downto 0);
      valid_out : out std_logic);
end component;
signal out_data1_inmem_pic : std_logic_vector(RAM_WIDTH - 1 downto 0);
signal out_data2_inmem_pic : std_logic_vector(RAM_WIDTH - 1 downto 0);
signal out_data3_inmem_pic : std_logic_vector(RAM_WIDTH - 1 downto 0);


signal out_data1_inmem_weight : std_logic_vector(RAM_WIDTH - 1 downto 0);
signal out_data2_inmem_weight : std_logic_vector(RAM_WIDTH - 1 downto 0);
signal out_data3_inmem_weight : std_logic_vector(RAM_WIDTH - 1 downto 0);

signal out_data1_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data2_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data3_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data4_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data5_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data6_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data7_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data8_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data9_cache_pic1 : std_logic_vector(RAM_WIDTH-1 downto 0);

signal out_data1_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data2_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data3_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data4_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data5_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data6_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data7_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data8_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data9_cache_pic23 : std_logic_vector(RAM_WIDTH-1 downto 0);

signal out_data1_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data2_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data3_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data4_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data5_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data6_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data7_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data8_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data9_cache_weights1 : std_logic_vector(RAM_WIDTH-1 downto 0);

signal out_data1_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data2_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data3_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data4_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data5_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data6_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data7_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data8_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);
signal out_data9_cache_weights23 : std_logic_vector(RAM_WIDTH-1 downto 0);

signal picture_params_signal : std_logic_vector(RAM_WIDTH*NUM_OF_LINES_IN_CACHE-1 downto 0);
signal weights_params_signal : std_logic_vector(RAM_WIDTH*NUM_OF_LINES_IN_CACHE-1 downto 0);

signal mac_out_signal : std_logic_vector(RAM_WIDTH*NUM_OF_LINES_IN_CACHE-1 downto 0);

signal output_pixel_signal : std_logic_vector(RAM_WIDTH-1 downto 0);

signal bias_out_signal : std_logic_vector(RAM_WIDTH-1 downto 0);

signal relu_signal : std_logic_vector(RAM_WIDTH-1 downto 0);

signal add_tree_valid_out:std_logic; 

signal write_address_out_mem_next: std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);
signal write_address_out_mem_reg: std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);

begin

process(clk)  
begin
    if(rising_edge(clk)) then
        if(reset_out_mem_counter = '1') then
            write_address_out_mem_reg<=(others => '0');
        else
            write_address_out_mem_reg<=write_address_out_mem_next;
        end if;
    end if;
end process;

process(add_tree_valid_out,write_address_out_mem_reg) begin
    if(add_tree_valid_out ='1') then
        
 write_address_out_mem_next<= std_logic_vector(unsigned(write_address_out_mem_reg) + to_unsigned(1,OUTPUT_PIC_ADDR_SIZE));
    else
        write_address_out_mem_next <= write_address_out_mem_reg;
end if;
end process;


input_picture_mem: BRAM_in_pic 
    generic map(RAM_WIDTH=>RAM_WIDTH, RAM_DEPTH=>INPUT_PIC_RAM_DEPTH, ADDR_SIZE=>INPUT_PIC_ADDR_SIZE)
    port map(clk=>clk,
             en=>en_inmem_pic, 
             we=>we_inmem_pic,
             addr_read1=>addr_read1_inmem_pic,
             addr_read2=>addr_read2_inmem_pic,
             addr_read3=>addr_read3_inmem_pic,
             addr_write=>addr_write_inmem_pic,
             data_in=>data_in_inmem_pic,
             data_out1=>out_data1_inmem_pic,
             data_out2=>out_data2_inmem_pic,
             data_out3=>out_data3_inmem_pic);
             
 input_weight_mem: BRAM_in_pic 
    generic map(RAM_WIDTH=>RAM_WIDTH,RAM_DEPTH=>INPUT_WEIGHT_RAM_DEPTH,ADDR_SIZE=>INPUT_WEIGHT_ADDR_SIZE)
    port map(clk=>clk,
             en=>en_inmem_weights, 
             we=>we_inmem_weights,
             addr_read1=>addr_read1_inmem_weights,
             addr_read2=>addr_read2_inmem_weights,
             addr_read3=>addr_read3_inmem_weights,
             addr_write=>addr_write_inmem_weights,
             data_in=>data_in_inmem_weights,
             data_out1=>out_data1_inmem_weight,
             data_out2=>out_data2_inmem_weight,
             data_out3=>out_data3_inmem_weight);
   
output_picture_mem: BRAM_out_pic 
    generic map(RAM_WIDTH=>RAM_WIDTH,RAM_DEPTH=>OUTPUT_PIC_RAM_DEPTH,ADDR_SIZE=>OUTPUT_PIC_ADDR_SIZE)
    port map(en=>en_outmem,
             we=> add_tree_valid_out,
             addr_read=>addr_read_outmem,
             addr_write=>write_address_out_mem_reg,
             data_in=>relu_signal, -- 0 if value is less than 0, 
             data_out=>data_outmem,
             clk=>clk);
             
bias_memory: BRAM_out_pic 
    generic map(RAM_WIDTH=>RAM_WIDTH,RAM_DEPTH=>INPUT_BIAS_DEPTH,ADDR_SIZE=>INPUT_BIAS_ADDR_SIZE)
    port map(en=>en_biasmem,
             we=>we_biasmem,
             addr_read=>addr_read_biasmem,
             addr_write=>addr_write_biasmem,
             data_in=>data_inbias, -- 0 if value is less than 0, 
             data_out=>bias_out_signal,
             clk=>clk);

 --input bias

cache_block_picture_inst_layer1:cache_block_picture
    generic map(WORD_SIZE=>RAM_WIDTH,LINE_LENGHT=>LINE_LENGHT_1,NUM_OF_LINES=>NUM_OF_LINES_IN_CACHE)
    port map(clk=>clk,
             reset=>reset_cachelck_pic1,
             en=>en_cacheblck_pic1,
             data_in1=>out_data1_inmem_pic,
             data_in2=>out_data2_inmem_pic,       
             data_in3=>out_data3_inmem_pic,
             data_out1=>out_data1_cache_pic1,
             data_out2=>out_data2_cache_pic1,
             data_out3=>out_data3_cache_pic1,
             data_out4=>out_data4_cache_pic1,
             data_out5=>out_data5_cache_pic1,
             data_out6=>out_data6_cache_pic1,
             data_out7=>out_data7_cache_pic1,
             data_out8=>out_data8_cache_pic1,
             data_out9=>out_data9_cache_pic1);
    
cache_block_picture_inst_layer23:cache_block_picture
    generic map(WORD_SIZE=>RAM_WIDTH,LINE_LENGHT=>LINE_LENGHT_2,NUM_OF_LINES=>NUM_OF_LINES_IN_CACHE)
    port map(clk=>clk,
             reset=>reset_cachelck_pic23,
             en=>en_cacheblck_pic23,
             data_in1=>out_data1_inmem_pic,
             data_in2=>out_data2_inmem_pic,
             data_in3=>out_data3_inmem_pic,
             data_out1=>out_data1_cache_pic23,
             data_out2=>out_data2_cache_pic23,
             data_out3=>out_data3_cache_pic23,
             data_out4=>out_data4_cache_pic23,
             data_out5=>out_data5_cache_pic23,
             data_out6=>out_data6_cache_pic23,
             data_out7=>out_data7_cache_pic23,
             data_out8=>out_data8_cache_pic23,
             data_out9=>out_data9_cache_pic23);

cache_block_weights_inst_layer1:cache_block_weights
    generic map(WORD_SIZE=>RAM_WIDTH,LINE_LENGHT=>LINE_LENGHT_1,NUM_OF_LINES=>NUM_OF_LINES_IN_CACHE)
    port map(clk=>clk,
             reset=>reset_cachelck_weights1,
             en1=>en1_cacheblck_weights1,
             en2=>en2_cacheblck_weights1,
             en3=>en3_cacheblck_weights1,
             mode=>mode_cacheblck_weights1,
             data_in1=>out_data1_inmem_weight,
             data_in2=>out_data2_inmem_weight,
             data_in3=>out_data3_inmem_weight,
             data_out1=>out_data1_cache_weights1,
             data_out2=>out_data2_cache_weights1,
             data_out3=>out_data3_cache_weights1,
             data_out4=>out_data4_cache_weights1,
             data_out5=>out_data5_cache_weights1,
             data_out6=>out_data6_cache_weights1,
             data_out7=>out_data7_cache_weights1,
             data_out8=>out_data8_cache_weights1,
             data_out9=>out_data9_cache_weights1);

cache_block_weights_inst_layer23: cache_block_weights
    generic map(WORD_SIZE=>RAM_WIDTH,LINE_LENGHT=>LINE_LENGHT_2,NUM_OF_LINES=>NUM_OF_LINES_IN_CACHE)
    port map(clk=>clk,
             reset=>reset_cachelck_weights23,
             en1=>en1_cacheblck_weights23,
             en2=>en2_cacheblck_weights23,
             en3=>en3_cacheblck_weights23,
             mode=>mode_cacheblck_weights23,
             data_in1=>out_data1_inmem_weight,
             data_in2=>out_data2_inmem_weight,
             data_in3=>out_data3_inmem_weight,
             data_out1=>out_data1_cache_weights23,
             data_out2=>out_data2_cache_weights23,
             data_out3=>out_data3_cache_weights23,
             data_out4=>out_data4_cache_weights23,
             data_out5=>out_data5_cache_weights23,
             data_out6=>out_data6_cache_weights23,
             data_out7=>out_data7_cache_weights23,
             data_out8=>out_data8_cache_weights23,
             data_out9=>out_data9_cache_weights23);

mac_module: MAC_top
    generic map(NUM_MACs=>9,WORD_SIZE=>RAM_WIDTH)
    port map(picture_params=>picture_params_signal,
             weights_params=>weights_params_signal,
             clk=>clk,
             reset=>reset_mac,
             en=>en_mac,  -- probaj da izbacis;
             outputs=>mac_out_signal);

add_tree_inst: add_tree
    generic map(NUM_TO_ADD=>9,WORD_SIZE=>RAM_WIDTH)
    port map(inputs_to_add=>mac_out_signal,
             bias=>bias_out_signal,
             add_enable=>en_add,
             clk=>clk,
             reset=>reset_add,
             output_pixel=>output_pixel_signal,
             valid_out=>add_tree_valid_out);

picture_params_signal <= out_data3_cache_pic1 & out_data2_cache_pic1 & out_data1_cache_pic1 & out_data6_cache_pic1 & out_data5_cache_pic1 & out_data4_cache_pic1 & out_data9_cache_pic1 & out_data8_cache_pic1 & out_data7_cache_pic1 when mode_choice = '0' 
                        else  out_data3_cache_pic23 & out_data2_cache_pic23 & out_data1_cache_pic23 & out_data6_cache_pic23 & out_data5_cache_pic23 & out_data4_cache_pic23 & out_data9_cache_pic23 & out_data8_cache_pic23 & out_data7_cache_pic23;
                                               
weights_params_signal <= out_data1_cache_weights1 & out_data2_cache_weights1 & out_data3_cache_weights1 & out_data4_cache_weights1 & out_data5_cache_weights1 & out_data6_cache_weights1 & out_data7_cache_weights1 & out_data8_cache_weights1 & out_data9_cache_weights1 when mode_choice = '0'
                        else out_data1_cache_weights23 & out_data2_cache_weights23 & out_data3_cache_weights23 & out_data4_cache_weights23 & out_data5_cache_weights23 & out_data6_cache_weights23 & out_data7_cache_weights23 & out_data8_cache_weights23 & out_data9_cache_weights23;
               
 relu_signal <=  output_pixel_signal when (signed(output_pixel_signal) > to_signed(0,16))
                 else (others=>'0');                   

end Behavioral;
