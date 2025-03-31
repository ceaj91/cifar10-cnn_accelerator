library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity controlpath is
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
        NUM_OF_LINES_IN_CACHE : integer := 9;
        
        COMMAND_REG_WIDTH: integer := 14;    
        FILTER_COUNTER_WIDTH: integer := 7;
        FILL_BUFF_COUNTER_WIDTH: integer := 7;
        CHANNEL_COUNTER_WIDTH : integer := 6;
        ROWS_COUNTER_WIDTH : integer := 6;
        COLUMNS_COUNTER_WIDTH : integer := 6
        
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
  input_command: in std_logic_vector(COMMAND_REG_WIDTH-1 downto 0);
  
  --interrupt
  end_command_int: out std_logic;
  

  ----AXI_MASTER_STREAM  signals
    axim_s_valid:out std_logic;
    axim_s_last:out std_logic;
    axim_s_ready:in std_logic
     );
end controlpath;


--ostalo je implementirati:

--citanje podataka, dodati komandu
--konvolucija 2 i 3

-- NEW_FILTER_1, INITIAL_LOADING_BUFFER_1, DO_CONV_1, ADD_TREE_1, FILL_FIRST_ROW_1, FILL_SECOND_ROW_1,FILL_OTHERS_ROW_1
architecture Behavioral of controlpath is
type accelerator_states_type is (GLOBAL_RESET, IDLE, LOAD_PARAMETARS, END_COMMAND, NEW_FILTER_0, INITIAL_LOADING_BUFFER_0, DO_CONV_0, ADD_TREE_0, FILL_FIRST_ROW_0, FILL_SECOND_ROW_0,FILL_OTHERS_ROW_0,READ_OUTPUT_DATA, NEW_FILTER_1, INITIAL_LOADING_BUFFER_1, DO_CONV_1, ADD_TREE_1, FILL_FIRST_ROW_1, FILL_SECOND_ROW_1,FILL_OTHERS_ROW_1,NEW_FILTER_2, INITIAL_LOADING_BUFFER_2, DO_CONV_2, ADD_TREE_2, FILL_FIRST_ROW_2, FILL_SECOND_ROW_2,FILL_OTHERS_ROW_2);
signal state_reg, state_next : accelerator_states_type;

--signal command_reg: std_logic_vector(11 downto 0) := "00000000000";
--signal command_reg: std_logic_vector(COMMAND_REG_WIDTH-1 downto 0);
--signal command_next: std_logic_vector(COMMAND_REG_WIDTH-1 downto 0);


--counters 
signal filter_counter_reg: std_logic_vector(FILTER_COUNTER_WIDTH-1 downto 0); -- 32 za 1.     32 za 2.    64 za 3.
signal filter_counter_next: std_logic_vector(FILTER_COUNTER_WIDTH-1 downto 0); 

signal fill_buff_counter_reg: std_logic_vector(FILL_BUFF_COUNTER_WIDTH-1 downto 0);  --  1001 za 0 i 32 = 10000
signal fill_buff_counter_next: std_logic_vector(FILL_BUFF_COUNTER_WIDTH-1 downto 0);

signal channel_counter_reg:std_logic_vector(CHANNEL_COUNTER_WIDTH-1 downto 0);  
signal channel_counter_next:std_logic_vector(CHANNEL_COUNTER_WIDTH-1 downto 0);  

signal rows_counter_reg: std_logic_vector(ROWS_COUNTER_WIDTH-1 downto 0); -- from 0 to 32, 6 bits
signal rows_counter_next: std_logic_vector(ROWS_COUNTER_WIDTH-1 downto 0); 

signal columns_counter_reg: std_logic_vector(COLUMNS_COUNTER_WIDTH-1 downto 0); -- from 0 to 32, 6 bits
signal columns_counter_next: std_logic_vector(COLUMNS_COUNTER_WIDTH-1 downto 0); 

signal data_sent_to_out_reg: std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1 downto 0);
signal data_sent_to_out_next: std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1 downto 0);

--registers for address of memories
signal addr_read1_inmem_pic_reg : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal addr_read1_inmem_pic_next : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);

signal addr_read2_inmem_pic_reg : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal addr_read2_inmem_pic_next : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);

signal addr_read3_inmem_pic_reg : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);
signal addr_read3_inmem_pic_next : std_logic_vector(INPUT_PIC_ADDR_SIZE-1  downto 0);


signal addr_read1_inmem_weights_reg :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal addr_read1_inmem_weights_next :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);

signal addr_read2_inmem_weights_reg : std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal addr_read2_inmem_weights_next : std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);

signal addr_read3_inmem_weights_reg :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);
signal addr_read3_inmem_weights_next :  std_logic_vector(INPUT_WEIGHT_ADDR_SIZE-1  downto 0);

signal addr_write_inmem_reg: std_logic_vector(INPUT_PIC_ADDR_SIZE-1 downto 0);
signal addr_write_inmem_next: std_logic_vector(INPUT_PIC_ADDR_SIZE-1 downto 0);

signal number_of_output_data_reg : std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1 downto 0);
signal number_of_output_data_next : std_logic_vector(OUTPUT_PIC_ADDR_SIZE-1 downto 0);

signal new_weights_arrived_reg : std_logic;
signal new_weights_arrived_next : std_logic;
begin
process(clk)  begin
    if(rising_edge(clk)) then
        if(input_command = "00010000000000") then
            new_weights_arrived_reg<='0';
            state_reg<=GLOBAL_RESET;
            filter_counter_reg <= (others => '0');  
            fill_buff_counter_reg<= (others => '0');  
            channel_counter_reg<= (others => '0');
            rows_counter_reg <= (others => '0');
            columns_counter_reg <= (others => '0');
            --reset memory addres registers
            addr_read1_inmem_pic_reg <= (others =>'0');
            addr_read2_inmem_pic_reg <= (others =>'0');
            addr_read3_inmem_pic_reg <= (others =>'0');
            addr_read1_inmem_weights_reg <= (others =>'0');
            addr_read2_inmem_weights_reg <= (others =>'0');
            addr_read3_inmem_weights_reg <= (others =>'0');
            --command_next<=input_command;
            addr_write_inmem_reg<=(others =>'0');
            
            data_sent_to_out_reg<=(others =>'0');
            --reset command reg(videcemo kako se ovo radi dodatno)
            data_sent_to_out_reg<=(others =>'0');
            
            number_of_output_data_reg <=(others =>'0'); 
            --command_reg <=  input_command;   
        else
            new_weights_arrived_reg<=new_weights_arrived_next;
            state_reg <= state_next;
            filter_counter_reg<=filter_counter_next;
            fill_buff_counter_reg <= fill_buff_counter_next;
            channel_counter_reg <= channel_counter_next;
            rows_counter_reg<=rows_counter_next;
            columns_counter_reg <= columns_counter_next;
            addr_read1_inmem_pic_reg<=addr_read1_inmem_pic_next;
            addr_read2_inmem_pic_reg<=addr_read2_inmem_pic_next;
            addr_read3_inmem_pic_reg<=addr_read3_inmem_pic_next;
            addr_read1_inmem_weights_reg <= addr_read1_inmem_weights_next;
            addr_read2_inmem_weights_reg <= addr_read2_inmem_weights_next;
            addr_read3_inmem_weights_reg <= addr_read3_inmem_weights_next;
            --command_reg<= command_next;
            addr_write_inmem_reg<=addr_write_inmem_next;
            data_sent_to_out_reg<=data_sent_to_out_next;
            number_of_output_data_reg<=number_of_output_data_next;
        end if;
     end if;
end process;

process(state_reg,new_weights_arrived_reg,number_of_output_data_reg,axim_s_ready,data_sent_to_out_reg,axis_s_data_in,addr_write_inmem_reg, input_command,axis_s_valid,axis_s_last, addr_read1_inmem_pic_reg,addr_read2_inmem_pic_reg,addr_read3_inmem_pic_reg,addr_read1_inmem_weights_reg,addr_read2_inmem_weights_reg,addr_read3_inmem_weights_reg, filter_counter_reg, fill_buff_counter_reg, channel_counter_reg, rows_counter_reg, columns_counter_reg) 
begin
    --default value for axi slave
    axis_s_ready <= '0';
    --add default signal value enable for input memory storage on 0
    
    --default value for PICTURE MEMORY
     en_inmem_pic <= '0';
     we_inmem_pic <= '0';
     addr_read1_inmem_pic_next <= addr_read1_inmem_pic_reg;
     addr_read2_inmem_pic_next <= addr_read2_inmem_pic_reg;
     addr_read3_inmem_pic_next <= addr_read3_inmem_pic_reg;
     addr_write_inmem_pic <= (others => '0');
     
    --default value for WEIGHTS MEMORY
     en_inmem_weights <= '0';
     we_inmem_weights <= '0';
     addr_read1_inmem_weights_next <= addr_read1_inmem_weights_reg;
     addr_read2_inmem_weights_next <= addr_read2_inmem_weights_reg;
     addr_read3_inmem_weights_next <= addr_read3_inmem_weights_reg;
     addr_write_inmem_weights <=  (others => '0');
     
    --default value for BIAS MEMORY
    en_biasmem <= '0';
    we_biasmem <= '0';
    addr_read_biasmem <= (others => '0');
    addr_write_biasmem <= (others => '0');
    --output map memory control
    en_outmem <='1';
    
    --default value for cache block pic 1
    en_cacheblck_pic1 <= '0';
    reset_cachelck_pic1<= '0';
    
      --default value for cache block pic 2 and 3
      en_cacheblck_pic23 <= '0';
      reset_cachelck_pic23<= '0';
  
      --default value for cache block weights 1
      en1_cacheblck_weights1 <= '0';
      en2_cacheblck_weights1 <= '0';
      en3_cacheblck_weights1 <= '0';
      mode_cacheblck_weights1<='1';
      reset_cachelck_weights1<= '0';
      
      --default value for cache block weights 2 and 3 
      en1_cacheblck_weights23 <= '0';
      en2_cacheblck_weights23 <= '0';
      en3_cacheblck_weights23 <= '0';
      mode_cacheblck_weights23<='1';
      reset_cachelck_weights23 <= '0';
  
      --MACs modules
      reset_mac <= '0';
      en_mac <= '0';
  
      --add tree 
      en_add <= '0';
      reset_add <= '0';
  
      --mode_choice = 0    - 1 layer         mode_choice = 1 - 2 and 3 layer
      mode_choice <= '0';
        
    --default values for counters, save value (ukoliko je potrebno restartovati brojace, to uraditi u odgovarajucim stanjima)   
    filter_counter_next <= filter_counter_reg;
    fill_buff_counter_next<= fill_buff_counter_reg; 
    channel_counter_next<= channel_counter_reg;
    rows_counter_next <= rows_counter_reg;
    columns_counter_next <= columns_counter_reg;
    addr_write_inmem_next <= addr_write_inmem_reg;
    new_weights_arrived_next <= new_weights_arrived_reg;

    --state_next <= GLOBAL_RESET;                            --POTREBNO JE OBRISATI !!!!!!!!!!!
    
    reset_out_mem_counter<='0';
    
    
    data_sent_to_out_next<=data_sent_to_out_reg;
    axim_s_valid<='0';
    axim_s_last<='0';
    end_command_int<= '0';
    number_of_output_data_next<=number_of_output_data_reg;
    case state_reg is
        when GLOBAL_RESET=>
        
            reset_cachelck_pic1 <= '1';
            reset_cachelck_pic23 <= '1';
            reset_cachelck_weights1 <= '1';
            reset_cachelck_weights23<='1';
            --reset mac 
            reset_mac <= '1';
            --reset add tree
            reset_add<='1';
            --reset counters 

            
            reset_out_mem_counter<='1';
           
           
            state_next <= IDLE;
           
            when IDLE =>
            --command_next<= input_command;
            --reset all counters  
            fill_buff_counter_next<= (others => '0');  
            channel_counter_next<= (others => '0');
            rows_counter_next <= (others => '0');
            columns_counter_next <= (others => '0');
            addr_write_inmem_next <= (others => '0');
            --reset cache blocks
            reset_cachelck_pic1 <= '1';
            reset_cachelck_pic23 <= '1';
            reset_cachelck_weights1 <= '1';
            reset_cachelck_weights23<='1';
            --reset mac 
            reset_mac <= '1';
            --reset add 
            reset_add<='0';
            
            addr_read1_inmem_weights_next <= std_logic_vector(TO_UNSIGNED(0,INPUT_WEIGHT_ADDR_SIZE));
            addr_read2_inmem_weights_next <= std_logic_vector(TO_UNSIGNED(1,INPUT_WEIGHT_ADDR_SIZE));
            addr_read3_inmem_weights_next <= std_logic_vector(TO_UNSIGNED(2,INPUT_WEIGHT_ADDR_SIZE));
            
            addr_read1_inmem_pic_next <= std_logic_vector(TO_UNSIGNED(0,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
            
            number_of_output_data_next <=(others =>'0');
            data_sent_to_out_next<=(others =>'0');
            new_weights_arrived_next <= '1';
            --determine next state
            if(input_command(0) = '1' or input_command(1) = '1'or input_command(2) = '1'or input_command(4) = '1'or input_command(5) = '1' or input_command(7) = '1' or input_command(8) = '1') then  -- load bias 
                state_next <= LOAD_PARAMETARS;
            elsif(input_command(3) = '1') then  -- do conv 0
                state_next <= NEW_FILTER_0;
            elsif(input_command(6) = '1') then
                state_next <= NEW_FILTER_1; -- do conv 1
            elsif(input_command(9) = '1') then
                state_next <= NEW_FILTER_2; -- do conv 2
            elsif(input_command(10) = '1') then
                state_next <= GLOBAL_RESET; -- do conv 2
            elsif(input_command(11) = '1') then
                state_next <= READ_OUTPUT_DATA; -- do conv 2
                number_of_output_data_next <= std_logic_vector(to_unsigned(32767,15));
                
            elsif(input_command(12) = '1') then
                state_next <= READ_OUTPUT_DATA; -- do conv 2
                number_of_output_data_next <= std_logic_vector(to_unsigned(8191,15));
            elsif(input_command(13) = '1') then
                state_next <= READ_OUTPUT_DATA; -- do conv 2
                number_of_output_data_next <= std_logic_vector(to_unsigned(4095,15));

            else
                state_next <= IDLE;
            end if;
        when LOAD_PARAMETARS =>
            axis_s_ready <= '1';
            --determine next state
            if(axis_s_valid = '1')then
                if(axis_s_last ='0') then
                    state_next <=LOAD_PARAMETARS;
                else
                    state_next <= END_COMMAND;
                end if;
                --outputs
                addr_write_inmem_next <= std_logic_vector(UNSIGNED(addr_write_inmem_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                
                
                if(input_command(0) = '1') then 
                    --enable for bias mem
                    en_biasmem <= '1';
                    we_biasmem <= '1';
                    --address
                    addr_write_biasmem <= addr_write_inmem_reg(INPUT_BIAS_ADDR_SIZE-1 downto 0);
                    --data
                elsif(input_command(1) = '1' or input_command(4) = '1' or input_command(7) = '1') then
                    --enable za weight 0
                    en_inmem_weights<='1';
                    we_inmem_weights<='1';
                    --address
                    addr_write_inmem_weights <= addr_write_inmem_reg(INPUT_WEIGHT_ADDR_SIZE-1 downto 0);
                    --data
                elsif(input_command(2) = '1'  or input_command(5) = '1' or input_command(8) = '1') then
                    --enable za picture 0
                    en_inmem_pic<='1';
                    we_inmem_pic<='1';
                    --address
                    addr_write_inmem_pic<=addr_write_inmem_reg; 
                    --data  
                end if;
            else
                state_next <= LOAD_PARAMETARS;
            end if;
            

        when NEW_FILTER_0 =>
             
            fill_buff_counter_next<= (others => '0');  
            channel_counter_next<= (others => '0');
            rows_counter_next <= (others => '0');
            columns_counter_next <= (others => '0');
            --reset counters for rows,columns,channel, reset cache and mac blocks
            reset_cachelck_pic1 <= '1';
            reset_cachelck_weights1 <= '1';
            reset_mac <= '1';
            
            
            
            addr_read1_inmem_pic_next <= std_logic_vector(TO_UNSIGNED(0,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                           
            if(filter_counter_reg = std_logic_vector(to_unsigned(32,FILTER_COUNTER_WIDTH))) then
                state_next <= END_COMMAND;
            else
                state_next <= INITIAL_LOADING_BUFFER_0;
            end if;
            
        when INITIAL_LOADING_BUFFER_0 =>
            en_inmem_pic <= '1';
            en_inmem_weights <= '1';
            --enable cache picture blocks 0
            en_cacheblck_pic1 <= '1';
            --settings for cache weights block
            mode_cacheblck_weights1<='0';--loading weights from mem in cache
            
            fill_buff_counter_next<= std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg < std_logic_vector(to_unsigned(9,FILL_BUFF_COUNTER_WIDTH))) then
                state_next<=INITIAL_LOADING_BUFFER_0;
                 
                addr_read1_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));    
                addr_read2_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));
                addr_read3_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));             
            else 
                state_next<=DO_CONV_0;
            end if;
            
            
            
            if(fill_buff_counter_reg <= std_logic_vector(to_unsigned(3,FILL_BUFF_COUNTER_WIDTH))) then
                en1_cacheblck_weights1 <= '1';
            elsif(fill_buff_counter_reg <= std_logic_vector(to_unsigned(6,FILL_BUFF_COUNTER_WIDTH))) then
                en2_cacheblck_weights1 <= '1';    
            else
                en3_cacheblck_weights1 <= '1';  
            end if;    

            
        when DO_CONV_0 =>
           channel_counter_next <= std_logic_vector(UNSIGNED(channel_counter_reg)+to_unsigned(1,CHANNEL_COUNTER_WIDTH));
           en_mac <= '1';
           if(channel_counter_reg < std_logic_vector(to_unsigned(3,CHANNEL_COUNTER_WIDTH))) then 
               state_next<= DO_CONV_0;
               en_inmem_pic <= '1';
               
               en_cacheblck_pic1 <= '1';
               en1_cacheblck_weights1 <= '1';
               en2_cacheblck_weights1 <= '1';
               en3_cacheblck_weights1 <= '1';
               if(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH)))  then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(2,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               else 
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(203,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(101,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
              end if;
           elsif(channel_counter_reg = std_logic_vector(to_unsigned(3,CHANNEL_COUNTER_WIDTH))) then
                state_next<=DO_CONV_0; 
           else
                state_next<=ADD_TREE_0;
                en_biasmem<='1';
                addr_read_biasmem <= std_logic_vector(unsigned(filter_counter_reg) + to_unsigned(0,INPUT_BIAS_ADDR_SIZE)); --filter_counter_reg counts how many counters are done, do not reset in same convolution stage              
               
           end if;
           
        when ADD_TREE_0 =>
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            channel_counter_next<=(others => '0'); -- reset channel counter
            en_add <='1';  --bias data and 9 data from mac are ready to be added
            reset_mac <= '1';  --mac reset
            columns_counter_next <= std_logic_vector(UNSIGNED(columns_counter_reg)+to_unsigned(1,COLUMNS_COUNTER_WIDTH));
            if(columns_counter_reg = std_logic_vector(to_unsigned(31,COLUMNS_COUNTER_WIDTH))) then
                rows_counter_next <= std_logic_vector(UNSIGNED(rows_counter_reg)+to_unsigned(1,ROWS_COUNTER_WIDTH));
                fill_buff_counter_next <= (others=>'0'); --restartuj brojac kako bi mogao da puni u stanjima fill row 1,2,others
                if(rows_counter_reg = std_logic_vector(to_unsigned(31,ROWS_COUNTER_WIDTH))) then
                    state_next <= NEW_FILTER_0;
                    filter_counter_next <= std_logic_vector(UNSIGNED(filter_counter_reg)+to_unsigned(1,FILTER_COUNTER_WIDTH)); -- zavrsio sa jednim filterom, prelazi se na sledeci
                    addr_read1_inmem_pic_next <= (others =>'0');
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_FIRST_ROW_0;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(306,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_SECOND_ROW_0;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(306,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(408,INPUT_PIC_ADDR_SIZE));
                else
                    state_next <= FILL_OTHERS_ROW_0;
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(208,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(106,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(4,INPUT_PIC_ADDR_SIZE));
                end if;
            else
                state_next <= DO_CONV_0;
            end if;
                    
        when FILL_FIRST_ROW_0=>
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic1 <= '1';
            en_inmem_pic <= '1';
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
                 
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(9,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_0;
            else
                state_next <= FILL_FIRST_ROW_0;                
            end if;
        when FILL_SECOND_ROW_0 =>
        fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic1 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(9,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_0;
       
            else
                state_next <= FILL_SECOND_ROW_0;                

            end if;
        when FILL_OTHERS_ROW_0 =>
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic1 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '1';
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(203,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(101,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(9,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_0;
               
            else
                state_next <= FILL_OTHERS_ROW_0;                
            end if; 
--------------------------------------------------------FIRST CONVOLUTION STATES----------------------------------------------------------------------------------------------------------------------------            
       when NEW_FILTER_1 =>
            mode_choice <= '1'; 
            fill_buff_counter_next<= (others => '0');  
            channel_counter_next<= (others => '0');
            rows_counter_next <= (others => '0');
            columns_counter_next <= (others => '0');
            --reset counters for rows,columns,channel, reset cache and mac blocks
            reset_cachelck_pic23 <= '1';
            reset_cachelck_weights23 <= '1';
            reset_mac <= '1';
            new_weights_arrived_next <= '0';

            
            
            addr_read1_inmem_pic_next <= std_logic_vector(TO_UNSIGNED(0,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                           
            if(filter_counter_reg = std_logic_vector(to_unsigned(16,FILTER_COUNTER_WIDTH)) or filter_counter_reg = std_logic_vector(to_unsigned(32,FILTER_COUNTER_WIDTH))) then
                if(new_weights_arrived_reg = '1') then
                    state_next <= INITIAL_LOADING_BUFFER_1;
                else
                    state_next <= END_COMMAND;
                end if;
            else
                state_next <= INITIAL_LOADING_BUFFER_1;
            end if;
            
        when INITIAL_LOADING_BUFFER_1 =>
            mode_choice <= '1';
            en_inmem_pic <= '1';
            en_inmem_weights <= '1';
            --enable cache picture blocks 0
            en_cacheblck_pic23 <= '1';
            --settings for cache weights block
            mode_cacheblck_weights23<='0';--loading weights from mem in cache
            
            fill_buff_counter_next<= std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg < std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next<=INITIAL_LOADING_BUFFER_1;
                 
                addr_read1_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));    
                addr_read2_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));
                addr_read3_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));             
            else 
                state_next<=DO_CONV_1;
            end if;
            
            
            
            if(fill_buff_counter_reg <= std_logic_vector(to_unsigned(32,FILL_BUFF_COUNTER_WIDTH))) then
                en1_cacheblck_weights23 <= '1';
            elsif(fill_buff_counter_reg <= std_logic_vector(to_unsigned(64,FILL_BUFF_COUNTER_WIDTH))) then
                en2_cacheblck_weights23 <= '1';    
            else
                en3_cacheblck_weights23 <= '1';  
            end if;    

            
        when DO_CONV_1 =>
            mode_choice <= '1';
           channel_counter_next <= std_logic_vector(UNSIGNED(channel_counter_reg)+to_unsigned(1,CHANNEL_COUNTER_WIDTH));
           en_mac <= '1';
           if(channel_counter_reg < std_logic_vector(to_unsigned(32,CHANNEL_COUNTER_WIDTH))) then 
               state_next<= DO_CONV_1;
               en_inmem_pic <= '1';
               
               en_cacheblck_pic23 <= '1';
               en1_cacheblck_weights23 <= '1';
               en2_cacheblck_weights23 <= '1';
               en3_cacheblck_weights23 <= '1';
               if(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH)))  then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(2,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               else 
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(1151,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(575,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));    -- provereno dobro
              end if;
           elsif(channel_counter_reg = std_logic_vector(to_unsigned(32,CHANNEL_COUNTER_WIDTH))) then
                state_next<=DO_CONV_1; 
           else
                state_next<=ADD_TREE_1;
                en_biasmem<='1';
                addr_read_biasmem <= std_logic_vector(unsigned(filter_counter_reg) + to_unsigned(32,INPUT_BIAS_ADDR_SIZE)); --filter_counter_reg counts how many counters are done, do not reset in same convolution stage              
               
           end if;
           
        when ADD_TREE_1 =>
            mode_choice <= '1';
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            channel_counter_next<=(others => '0'); -- reset channel counter
            en_add <='1';  --bias data and 9 data from mac are ready to be added
            reset_mac <= '1';  --mac reset
            columns_counter_next <= std_logic_vector(UNSIGNED(columns_counter_reg)+to_unsigned(1,COLUMNS_COUNTER_WIDTH));
            if(columns_counter_reg = std_logic_vector(to_unsigned(15,COLUMNS_COUNTER_WIDTH))) then
                rows_counter_next <= std_logic_vector(UNSIGNED(rows_counter_reg)+to_unsigned(1,ROWS_COUNTER_WIDTH));
                fill_buff_counter_next <= (others=>'0'); --restartuj brojac kako bi mogao da puni u stanjima fill row 1,2,others
                if(rows_counter_reg = std_logic_vector(to_unsigned(15,ROWS_COUNTER_WIDTH))) then
                    state_next <= NEW_FILTER_1;
                    filter_counter_next <= std_logic_vector(UNSIGNED(filter_counter_reg)+to_unsigned(1,FILTER_COUNTER_WIDTH)); -- zavrsio sa jednim filterom, prelazi se na sledeci
                    addr_read1_inmem_pic_next <= (others =>'0');
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_FIRST_ROW_1;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(1728,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_SECOND_ROW_1;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1728,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2304,INPUT_PIC_ADDR_SIZE));
                else
                    state_next <= FILL_OTHERS_ROW_1;
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(1185,INPUT_PIC_ADDR_SIZE));   -- ovde je bio bag, napisi na msg, mora -32 svuda u odnosu na VP, jer dok se radi poslednja kovnolucija, generisu se adrese idalje, puni se kes, samim tim se i adresni registi povecavaju. Posto ima 32 takta, adresni registri su za 32 veci, za conv_0 to je slucaj 3, jos se dodaje 1 jer conv ostaje takt dodatno u 
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(609,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(33,INPUT_PIC_ADDR_SIZE));  -- bilo je -4 stavio sam na -32,mozda -33
                end if;
            else
                state_next <= DO_CONV_1;
            end if;
                    
        when FILL_FIRST_ROW_1=>
            mode_choice <= '1';
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
                 
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_1;
            else
                state_next <= FILL_FIRST_ROW_1;                
            end if;
        when FILL_SECOND_ROW_1 =>
        mode_choice <= '1';
        fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_1;
       
            else
                state_next <= FILL_SECOND_ROW_1;                

            end if;
        when FILL_OTHERS_ROW_1 =>
            mode_choice <= '1';
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '1';
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(1151,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(575,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_1;
               
            else
                state_next <= FILL_OTHERS_ROW_1;                
            end if; 
 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------    
         when NEW_FILTER_2 =>
            mode_choice <= '1'; 
            fill_buff_counter_next<= (others => '0');  
            channel_counter_next<= (others => '0');
            rows_counter_next <= (others => '0');
            columns_counter_next <= (others => '0');
            --reset counters for rows,columns,channel, reset cache and mac blocks
            reset_cachelck_pic23 <= '1';
            reset_cachelck_weights23 <= '1';
            reset_mac <= '1';
            new_weights_arrived_next <= '0';
            
            
            addr_read1_inmem_pic_next <= std_logic_vector(TO_UNSIGNED(0,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                           
            if(filter_counter_reg = std_logic_vector(to_unsigned(16,FILTER_COUNTER_WIDTH)) or filter_counter_reg = std_logic_vector(to_unsigned(32,FILTER_COUNTER_WIDTH)) or filter_counter_reg = std_logic_vector(to_unsigned(48,FILTER_COUNTER_WIDTH)) or filter_counter_reg = std_logic_vector(to_unsigned(64,FILTER_COUNTER_WIDTH))) then
                 if(new_weights_arrived_reg = '1') then
                    state_next <= INITIAL_LOADING_BUFFER_2;
                else
                    state_next <= END_COMMAND;
                end if;
            else
                state_next <= INITIAL_LOADING_BUFFER_2;
            end if;
            
            
        when INITIAL_LOADING_BUFFER_2 =>
        mode_choice <= '1';
            en_inmem_pic <= '1';
            en_inmem_weights <= '1';
            --enable cache picture blocks 0
            en_cacheblck_pic23 <= '1';
            --settings for cache weights block
            mode_cacheblck_weights23<='0';--loading weights from mem in cache
            
            fill_buff_counter_next<= std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg < std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next<=INITIAL_LOADING_BUFFER_2;
                 
                addr_read1_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));    
                addr_read2_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));
                addr_read3_inmem_weights_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_weights_reg)+to_unsigned(3,INPUT_WEIGHT_ADDR_SIZE));             
            else 
                state_next<=DO_CONV_2;
            end if;
            
            
            
            if(fill_buff_counter_reg <= std_logic_vector(to_unsigned(32,FILL_BUFF_COUNTER_WIDTH))) then
                en1_cacheblck_weights23 <= '1';
            elsif(fill_buff_counter_reg <= std_logic_vector(to_unsigned(64,FILL_BUFF_COUNTER_WIDTH))) then
                en2_cacheblck_weights23 <= '1';    
            else
                en3_cacheblck_weights23 <= '1';  
            end if;    

            
        when DO_CONV_2 =>   
           mode_choice <= '1'; 
           channel_counter_next <= std_logic_vector(UNSIGNED(channel_counter_reg)+to_unsigned(1,CHANNEL_COUNTER_WIDTH));
           en_mac <= '1';
           if(channel_counter_reg < std_logic_vector(to_unsigned(32,CHANNEL_COUNTER_WIDTH))) then 
               state_next<= DO_CONV_2;
               en_inmem_pic <= '1';
               
               en_cacheblck_pic23 <= '1';
               en1_cacheblck_weights23 <= '1';
               en2_cacheblck_weights23 <= '1';
               en3_cacheblck_weights23 <= '1';
               if(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH)))  then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               elsif(rows_counter_reg = std_logic_vector(to_unsigned(2,ROWS_COUNTER_WIDTH))) then
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
               else 
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(639,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(319,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
              end if;
           elsif(channel_counter_reg = std_logic_vector(to_unsigned(32,CHANNEL_COUNTER_WIDTH))) then
                state_next<=DO_CONV_2; 
           else
                state_next<=ADD_TREE_2;
                en_biasmem<='1';
                addr_read_biasmem <= std_logic_vector(unsigned(filter_counter_reg) + to_unsigned(64,INPUT_BIAS_ADDR_SIZE)); --filter_counter_reg counts how many counters are done, do not reset in same convolution stage              
               
           end if;
           
        when ADD_TREE_2 =>
            mode_choice <= '1';
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            channel_counter_next<=(others => '0'); -- reset channel counter
            en_add <='1';  --bias data and 9 data from mac are ready to be added
            reset_mac <= '1';  --mac reset
            columns_counter_next <= std_logic_vector(UNSIGNED(columns_counter_reg)+to_unsigned(1,COLUMNS_COUNTER_WIDTH));
            if(columns_counter_reg = std_logic_vector(to_unsigned(7,COLUMNS_COUNTER_WIDTH))) then
                rows_counter_next <= std_logic_vector(UNSIGNED(rows_counter_reg)+to_unsigned(1,ROWS_COUNTER_WIDTH));
                fill_buff_counter_next <= (others=>'0'); --restartuj brojac kako bi mogao da puni u stanjima fill row 1,2,others
                if(rows_counter_reg = std_logic_vector(to_unsigned(7,ROWS_COUNTER_WIDTH))) then
                    state_next <= NEW_FILTER_2;
                    filter_counter_next <= std_logic_vector(UNSIGNED(filter_counter_reg)+to_unsigned(1,FILTER_COUNTER_WIDTH)); -- zavrsio sa jednim filterom, prelazi se na sledeci
                    addr_read1_inmem_pic_next <= (others =>'0');
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(0,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_FIRST_ROW_2;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(1,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(960,INPUT_PIC_ADDR_SIZE));
                elsif(rows_counter_reg = std_logic_vector(to_unsigned(1,ROWS_COUNTER_WIDTH))) then
                    state_next <= FILL_SECOND_ROW_2;
                    addr_read1_inmem_pic_next <= std_logic_vector(to_unsigned(2,INPUT_PIC_ADDR_SIZE));
                    addr_read2_inmem_pic_next <= std_logic_vector(to_unsigned(960,INPUT_PIC_ADDR_SIZE));
                    addr_read3_inmem_pic_next <= std_logic_vector(to_unsigned(1280,INPUT_PIC_ADDR_SIZE));
                else
                    state_next <= FILL_OTHERS_ROW_2;
                    addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(673,INPUT_PIC_ADDR_SIZE)); -- potencijalno problem 
                    addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(353,INPUT_PIC_ADDR_SIZE)); -- potencijalno problem
                    addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(33,INPUT_PIC_ADDR_SIZE));  -- potencijalno problem
                end if;
            else
                state_next <= DO_CONV_2;
            end if;
                    
        when FILL_FIRST_ROW_2=>
            mode_choice <= '1';
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
            --en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
                 
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_2;
            else
                state_next <= FILL_FIRST_ROW_2;                
            end if;
        when FILL_SECOND_ROW_2 =>
        mode_choice <= '1';
        fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '0'; -- blok ne radi nista, on ce raditi samo u do_conv_0
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read1_inmem_pic_reg)+to_unsigned(3,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read2_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_2;
       
            else
                state_next <= FILL_SECOND_ROW_2;                

            end if;
        when FILL_OTHERS_ROW_2 =>
            mode_choice <= '1';
            fill_buff_counter_next <=  std_logic_vector(UNSIGNED(fill_buff_counter_reg)+to_unsigned(1,FILL_BUFF_COUNTER_WIDTH));
            en_cacheblck_pic23 <= '1';
            en_inmem_pic <= '1';
           -- en_cacheblck_weights1 <= '1';
            columns_counter_next <= (others => '0');
            addr_read1_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(639,INPUT_PIC_ADDR_SIZE));
            addr_read2_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)-to_unsigned(319,INPUT_PIC_ADDR_SIZE));
            addr_read3_inmem_pic_next <= std_logic_vector(UNSIGNED(addr_read3_inmem_pic_reg)+to_unsigned(1,INPUT_PIC_ADDR_SIZE));
            if(fill_buff_counter_reg = std_logic_vector(to_unsigned(96,FILL_BUFF_COUNTER_WIDTH))) then
                state_next <= DO_CONV_2;
               
            else
                state_next <= FILL_OTHERS_ROW_2;                
            end if; 
--------------------------------------------------------------------------------------------------------------
        when READ_OUTPUT_DATA=>
            --next state determine
            
            
            if(data_sent_to_out_reg < number_of_output_data_reg) then
                state_next<=READ_OUTPUT_DATA;
            else
                if(axim_s_ready = '1') then
                    state_next<=END_COMMAND;
                    axim_s_last <= '1';
                else 
                    state_next<=READ_OUTPUT_DATA;
                    axim_s_last <= '1';
                end if;
            end if;
            
            axim_s_valid<='1';
            if(axim_s_ready = '1') then
                data_sent_to_out_next <= std_logic_vector(UNSIGNED(data_sent_to_out_reg)+to_unsigned(1,OUTPUT_PIC_ADDR_SIZE));
            end if;
            
            
        when END_COMMAND =>
            --command_next<=(others=>'0');
            end_command_int<= '1';
            state_next <= IDLE;
            --interupt
    end case;  
end process;

addr_read1_inmem_pic <=addr_read1_inmem_pic_reg;
addr_read2_inmem_pic <=addr_read2_inmem_pic_reg;
addr_read3_inmem_pic <=addr_read3_inmem_pic_reg;

addr_read1_inmem_weights <= addr_read1_inmem_weights_reg;
addr_read2_inmem_weights <= addr_read2_inmem_weights_reg;
addr_read3_inmem_weights<= addr_read3_inmem_weights_reg;

addr_read_outmem<=data_sent_to_out_next;

data_inbias<=axis_s_data_in;
data_in_inmem_weights <= axis_s_data_in;
data_in_inmem_pic <= axis_s_data_in;
end Behavioral;
