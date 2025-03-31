

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity cache_block_weights_tb is
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 3;
        NUM_OF_LINES: integer := 9);
--  Port ( );
end cache_block_weights_tb;

architecture Behavioral of cache_block_weights_tb is
signal clk: std_logic;
signal reset: std_logic;
signal en1:  std_logic;
signal en2:  std_logic;
signal en3:  std_logic;
signal mode:  std_logic;   -- 0-initial filling buffer      1 - circular buffer
signal data_in1: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_in2: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_in3: std_logic_vector(WORD_SIZE-1 downto 0);

signal data_out1: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out2: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out3: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out4: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out5: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out6: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out7: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out8: std_logic_vector(WORD_SIZE-1 downto 0);
signal data_out9: std_logic_vector(WORD_SIZE-1 downto 0);

begin

duv: entity work.cache_block_weights
generic map(WORD_SIZE=>WORD_SIZE,LINE_LENGHT=>LINE_LENGHT,NUM_OF_LINES=>NUM_OF_LINES)
port map(    clk=> clk,
             reset=> reset,
             en1=> en1,
             en2=> en2,
             en3=> en3,
             mode=> mode,   -- 0-initial filling buffer      1 - circular buffer
             data_in1=> data_in1,
             data_in2=> data_in2,
             data_in3=> data_in3,
             data_out1=> data_out1,
             data_out2=>data_out2 ,
             data_out3=>data_out3 ,
             data_out4=>data_out4 ,
             data_out5=>data_out5 ,
             data_out6=> data_out6,
             data_out7=>data_out7 ,
             data_out8=>data_out8 ,
             data_out9=> data_out9);

clk_gen: process is begin
    clk <= '0';
    wait for 5 ns;
    clk <= '1';
    wait for 5 ns;
end process;

stim_gen : process is begin
    
    reset<='1';
    mode<='0';
    en1<='0';
    en2<='0';
    en3<='0';
    data_in1<=(others => '0');
    data_in2<=(others => '0');
    data_in3<=(others => '0');
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"1111";
    data_in2<=x"2222";
    data_in3<=x"3333";
    en1<='1';
    en2<='0';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"4444";
    data_in2<=x"5555";
    data_in3<=x"6666";
    en1<='1';
    en2<='0';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"7777";
    data_in2<=x"8888";
    data_in3<=x"9999";
    en1<='1';
    en2<='0';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"AAAA";
    data_in2<=x"BBBB";
    data_in3<=x"CCCC";
    en1<='0';
    en2<='1';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"FFFF";
    data_in2<=x"0000";
    data_in3<=x"1111";
    en1<='0';
    en2<='1';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"2222";
    data_in2<=x"3333";
    data_in3<=x"4444";
    en1<='0';
    en2<='1';
    en3<='0';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"5555";
    data_in2<=x"6666";
    data_in3<=x"7777";
    en1<='0';
    en2<='0';
    en3<='1';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"8888";
    data_in2<=x"9999";
    data_in3<=x"AAAA";
    en1<='0';
    en2<='0';
    en3<='1';
    
    wait for 10ns;
    
    mode<='0';
    reset<='0';
    data_in1<=x"BBBB";
    data_in2<=x"CCCC";
    data_in3<=x"DDDD";
    en1<='0';
    en2<='0';
    en3<='1';
    
    wait for 10ns;
    en1<='1';
    en2<='1';
    en3<='1';
    mode<='1';
    wait;
    
end process;
end Behavioral;
