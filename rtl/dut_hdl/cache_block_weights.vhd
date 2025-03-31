library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity cache_block_weights is
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
end cache_block_weights;

architecture Behavioral of cache_block_weights is
signal end_line1: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line2: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line3: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line4: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line5: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line6: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line7: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line8: std_logic_vector(WORD_SIZE-1 downto 0);
signal end_line9: std_logic_vector(WORD_SIZE-1 downto 0);

signal start_line1: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line2: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line3: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line4: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line5: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line6: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line7: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line8: std_logic_vector(WORD_SIZE-1 downto 0);
signal start_line9: std_logic_vector(WORD_SIZE-1 downto 0);

component line_fifo_buffer 
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 32);
Port (clk:in std_logic;
      reset:in std_logic;
      en_line: in std_logic;
      data_in: in std_logic_vector(WORD_SIZE-1 downto 0); 
      data_out: out std_logic_vector(WORD_SIZE-1 downto 0));
end component; 

begin

process(mode,data_in1,data_in2,data_in3,end_line1,end_line2,end_line3,end_line4,end_line5,end_line6,end_line7,end_line8,end_line9)  begin
    if(mode = '0') then
        start_line1 <= data_in1; 
        start_line2 <= data_in2; 
        start_line3 <= data_in3; 
        start_line4 <= data_in1; 
        start_line5 <= data_in2; 
        start_line6 <= data_in3; 
        start_line7 <= data_in1; 
        start_line8 <= data_in2; 
        start_line9 <= data_in3; 
    else
        start_line1 <= end_line1; 
        start_line2 <= end_line2; 
        start_line3 <= end_line3; 
        start_line4 <= end_line4; 
        start_line5 <= end_line5; 
        start_line6 <= end_line6; 
        start_line7 <= end_line7; 
        start_line8 <= end_line8; 
        start_line9 <= end_line9;
    end if;

end process;
    
    line1: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en1,
             data_in=> start_line1, 
             data_out=>end_line1);
             
    line2: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en1,
             data_in=> start_line2, 
             data_out=>end_line2);
    line3: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en1,
             data_in=> start_line3, 
             data_out=>end_line3);   
    
    line4: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en2,
             data_in=> start_line4, 
             data_out=>end_line4);
             
    line5: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en2,
             data_in=> start_line5, 
             data_out=>end_line5);
    line6: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en2,
             data_in=> start_line6, 
             data_out=>end_line6);
    line7:line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en3,
             data_in=> start_line7, 
             data_out=>end_line7);
             
    line8: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en3,
             data_in=> start_line8, 
             data_out=>end_line8);
    line9: line_fifo_buffer
    generic map(WORD_SIZE=>WORD_SIZE,
                LINE_LENGHT=>LINE_LENGHT)
    port map(clk => clk,
             reset=>reset,
             en_line=>en3,
             data_in=> start_line9, 
             data_out=>end_line9);
             
             
             
     
    data_out1 <= end_line1;
    data_out2 <= end_line2;
    data_out3 <= end_line3;
    data_out4 <= end_line4;
    data_out5 <= end_line5;
    data_out6 <= end_line6;
    data_out7 <= end_line7;
    data_out8 <= end_line8;
    data_out9 <= end_line9;
    

end Behavioral;
