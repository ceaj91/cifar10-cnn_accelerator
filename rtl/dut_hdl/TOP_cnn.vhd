

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity TOP_cnn is
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
end TOP_cnn;

architecture Behavioral of TOP_cnn is


--input map memmory control
signal en_inmem_pic_s:  std_logic;
signal     we_inmem_pic_s:  std_logic;
signal     addr_read1_inmem_pic_s :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_read2_inmem_pic_s :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_read3_inmem_pic_s :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     addr_write_inmem_pic_s :  std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal     data_in_inmem_pic_s :  std_logic_vector(RAM_WIDTH - 1 downto 0);
     
 --input weights 
        
signal     en_inmem_weights_s:  std_logic;
signal     we_inmem_weights_s:  std_logic;
signal     addr_read1_inmem_weights_s :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     addr_read2_inmem_weights_s :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     addr_read3_inmem_weights_s :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     addr_write_inmem_weights_s :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal     data_in_inmem_weights_s :  std_logic_vector(RAM_WIDTH - 1 downto 0);
 
 --input bias
signal    en_biasmem_s:  std_logic;
signal    we_biasmem_s:  std_logic;
signal    addr_read_biasmem_s :  std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
signal    addr_write_biasmem_s:  std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
signal    data_inbias_s :  std_logic_vector(RAM_WIDTH - 1 downto 0);
 
 --output map memory control
signal    en_outmem_s:  std_logic;
signal    addr_read_outmem_s :  std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);
signal    reset_out_mem_counter_s:  std_logic;
  --cache block pic 1
signal  en_cacheblck_pic1_s :  std_logic;
signal  reset_cachelck_pic1_s:  std_logic;
  
  --cache block pic 2 and 3
signal  en_cacheblck_pic23_s :  std_logic;
signal  reset_cachelck_pic23_s:  std_logic;
  
  --cache block weights 1
signal  en1_cacheblck_weights1_s :  std_logic;
signal  en2_cacheblck_weights1_s :  std_logic;
signal  en3_cacheblck_weights1_s :  std_logic;
signal  mode_cacheblck_weights1_s :  std_logic;
signal  reset_cachelck_weights1_s:  std_logic;

  --cache block weights 2 and 3 
signal  en1_cacheblck_weights23_s :  std_logic;
signal  en2_cacheblck_weights23_s:  std_logic;
signal  en3_cacheblck_weights23_s:  std_logic;
signal  mode_cacheblck_weights23_s :  std_logic;
signal  reset_cachelck_weights23_s:  std_logic;
  
  --MACs modules
signal  reset_mac_s:  std_logic;
signal  en_mac_s :  std_logic;
  
  --add tree 
 signal en_add_s :  std_logic;
 signal reset_add_s:  std_logic;
  
  --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
 signal mode_choice_s :  std_logic;


component datapath_cnn
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
end component;

component controlpath
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
    --input map memmory control
     en_inmem_pic: out std_logic;
     we_inmem_pic: out std_logic;
     addr_read1_inmem_pic : out std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_read2_inmem_pic : out std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_read3_inmem_pic : out std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     addr_write_inmem_pic : out std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
     data_in_inmem_pic : out std_logic_vector(RAM_WIDTH - 1 downto 0); 
     --input weights
     en_inmem_weights: out std_logic;
     we_inmem_weights: out std_logic;
     addr_read1_inmem_weights : out std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_read2_inmem_weights : out std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_read3_inmem_weights : out std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     addr_write_inmem_weights : out std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
     data_in_inmem_weights : out std_logic_vector(RAM_WIDTH - 1 downto 0);
      --input bias
    en_biasmem: out std_logic;
    we_biasmem: out std_logic;
    addr_read_biasmem : out std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
    addr_write_biasmem: out std_logic_vector(INPUT_BIAS_ADDR_SIZE-1  downto 0);
    data_inbias : out std_logic_vector(RAM_WIDTH - 1 downto 0);
     --output map memory control
    en_outmem: out std_logic;
    addr_read_outmem : out std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1  downto 0);
    reset_out_mem_counter: out std_logic;
      --cache block pic 1
  en_cacheblck_pic1 : out std_logic;
  reset_cachelck_pic1: out std_logic;
  
  --cache block pic 2 and 3
  en_cacheblck_pic23 : out std_logic;
  reset_cachelck_pic23: out std_logic;
  
  --cache block weights 1
  en1_cacheblck_weights1 : out std_logic;
  en2_cacheblck_weights1 : out std_logic;
  en3_cacheblck_weights1 : out std_logic;
  mode_cacheblck_weights1 : out std_logic;
  reset_cachelck_weights1: out std_logic;
  
  --cache block weights 2 and 3 
  en1_cacheblck_weights23 : out std_logic;
  en2_cacheblck_weights23 : out std_logic;
  en3_cacheblck_weights23 : out std_logic;
  mode_cacheblck_weights23 : out std_logic;
  reset_cachelck_weights23: out std_logic;
  
  --MACs modules
  reset_mac: out std_logic;
  en_mac : out std_logic;
  
  --add tree 
  en_add : out std_logic;
  reset_add: out std_logic;
  
  --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
  mode_choice : out std_logic;
    
   --command_reg input
  input_command: in std_logic_vector(13 downto 0);
  
  --interrupt
  end_command_int: out std_logic;
  

  ----AXI_MASTER_STREAM  signals
    axim_s_valid:out std_logic;
    axim_s_last:out std_logic;
    axim_s_ready:in std_logic
     );
end component;
begin

datapath_inst: datapath_cnn
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
port map(clk =>clk ,
    --input map memmory control
         en_inmem_pic => en_inmem_pic_s,
         we_inmem_pic =>we_inmem_pic_s ,
         addr_read1_inmem_pic =>addr_read1_inmem_pic_s ,
         addr_read2_inmem_pic => addr_read2_inmem_pic_s, 
         addr_read3_inmem_pic  => addr_read3_inmem_pic_s,
         addr_write_inmem_pic  =>addr_write_inmem_pic_s ,
         data_in_inmem_pic  =>data_in_inmem_pic_s ,
         
     --input weights 
            
         en_inmem_weights => en_inmem_weights_s,
         we_inmem_weights =>we_inmem_weights_s ,
         addr_read1_inmem_weights => addr_read1_inmem_weights_s, 
         addr_read2_inmem_weights  => addr_read2_inmem_weights_s,
         addr_read3_inmem_weights =>addr_read3_inmem_weights_s , 
         addr_write_inmem_weights => addr_write_inmem_weights_s, 
         data_in_inmem_weights => data_in_inmem_weights_s,
     
     --input bias
        en_biasmem => en_biasmem_s,
        we_biasmem => we_biasmem_s,
        addr_read_biasmem => addr_read_biasmem_s, 
        addr_write_biasmem => addr_write_biasmem_s,
        data_inbias  => data_inbias_s,
     
     --output map memory control
        en_outmem => en_outmem_s,
        addr_read_outmem => addr_read_outmem_s, 
        data_outmem  => axim_s_data,
        reset_out_mem_counter => reset_out_mem_counter_s,
      --cache block pic 1
      en_cacheblck_pic1 => en_cacheblck_pic1_s,
      reset_cachelck_pic1 => reset_cachelck_pic1_s,
      
      --cache block pic 2 and 3
      en_cacheblck_pic23  => en_cacheblck_pic23_s,
      reset_cachelck_pic23 => reset_cachelck_pic23_s,
      
      --cache block weights 1
      en1_cacheblck_weights1 => en1_cacheblck_weights1_s,
      en2_cacheblck_weights1  => en2_cacheblck_weights1_s,
      en3_cacheblck_weights1  => en3_cacheblck_weights1_s,
      mode_cacheblck_weights1 => mode_cacheblck_weights1_s,
      reset_cachelck_weights1 => reset_cachelck_weights1_s,
    
      --cache block weights 2 and 3 
      en1_cacheblck_weights23 => en1_cacheblck_weights23_s,
      en2_cacheblck_weights23  => en2_cacheblck_weights23_s,
      en3_cacheblck_weights23  => en3_cacheblck_weights23_s,
      mode_cacheblck_weights23  => mode_cacheblck_weights23_s,
      reset_cachelck_weights23 => reset_cachelck_weights23_s,
      
      --MACs modules
      reset_mac => reset_mac_s,
      en_mac  =>en_mac_s ,
      
      --add tree 
      en_add  =>en_add_s ,
      reset_add => reset_add_s,
      
      --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
      mode_choice =>mode_choice_s);


controlpath_inst: controlpath
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
port map(clk =>clk ,
    --AXI_SLAVE_STREAM signals
    axis_s_data_in=>axis_s_data_in,
    axis_s_valid=>axis_s_valid,
    axis_s_last=>axis_s_last,
    axis_s_ready=>axis_s_ready,
    --input map memmory control
         en_inmem_pic => en_inmem_pic_s,
         we_inmem_pic =>we_inmem_pic_s ,
         addr_read1_inmem_pic =>addr_read1_inmem_pic_s ,
         addr_read2_inmem_pic => addr_read2_inmem_pic_s, 
         addr_read3_inmem_pic  => addr_read3_inmem_pic_s,
         addr_write_inmem_pic  =>addr_write_inmem_pic_s ,
         data_in_inmem_pic  =>data_in_inmem_pic_s ,
         
     --input weights 
            
         en_inmem_weights => en_inmem_weights_s,
         we_inmem_weights =>we_inmem_weights_s ,
         addr_read1_inmem_weights => addr_read1_inmem_weights_s, 
         addr_read2_inmem_weights  => addr_read2_inmem_weights_s,
         addr_read3_inmem_weights =>addr_read3_inmem_weights_s , 
         addr_write_inmem_weights => addr_write_inmem_weights_s, 
         data_in_inmem_weights => data_in_inmem_weights_s,
     
     --input bias
        en_biasmem => en_biasmem_s,
        we_biasmem => we_biasmem_s,
        addr_read_biasmem => addr_read_biasmem_s, 
        addr_write_biasmem => addr_write_biasmem_s,
        data_inbias  => data_inbias_s,
     
     --output map memory control
        en_outmem => en_outmem_s,
        addr_read_outmem => addr_read_outmem_s, 
        reset_out_mem_counter => reset_out_mem_counter_s,
      --cache block pic 1
      en_cacheblck_pic1 => en_cacheblck_pic1_s,
      reset_cachelck_pic1 => reset_cachelck_pic1_s,
      
      --cache block pic 2 and 3
      en_cacheblck_pic23  => en_cacheblck_pic23_s,
      reset_cachelck_pic23 => reset_cachelck_pic23_s,
      
      --cache block weights 1
      en1_cacheblck_weights1 => en1_cacheblck_weights1_s,
      en2_cacheblck_weights1  => en2_cacheblck_weights1_s,
      en3_cacheblck_weights1  => en3_cacheblck_weights1_s,
      mode_cacheblck_weights1 => mode_cacheblck_weights1_s,
      reset_cachelck_weights1 => reset_cachelck_weights1_s,
    
      --cache block weights 2 and 3 
      en1_cacheblck_weights23 => en1_cacheblck_weights23_s,
      en2_cacheblck_weights23  => en2_cacheblck_weights23_s,
      en3_cacheblck_weights23  => en3_cacheblck_weights23_s,
      mode_cacheblck_weights23  => mode_cacheblck_weights23_s,
      reset_cachelck_weights23 => reset_cachelck_weights23_s,
      
      --MACs modules
      reset_mac => reset_mac_s,
      en_mac  =>en_mac_s ,
      
      --add tree 
      en_add  =>en_add_s ,
      reset_add => reset_add_s,
      
      --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
      mode_choice =>mode_choice_s,
          --command_reg input
      input_command =>input_command ,
      
      --interrupt
      end_command_int => end_command_int,
      
    
      ----AXI_MASTER_STREAM  signals
        axim_s_valid => axim_s_valid,
        axim_s_last => axim_s_last,
        axim_s_ready => axim_s_ready);



end Behavioral;
