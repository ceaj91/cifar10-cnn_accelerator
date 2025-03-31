

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity cache_block_picture_tb is
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 3;
        NUM_OF_LINES: integer := 9);
--  Port ( );
end cache_block_picture_tb;

architecture Behavioral of cache_block_picture_tb is
signal clk: std_logic;
signal reset: std_logic;
signal en:  std_logic;
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

duv: entity work.cache_block_picture
generic map(WORD_SIZE=>WORD_SIZE,LINE_LENGHT=>LINE_LENGHT,NUM_OF_LINES=>NUM_OF_LINES)
port map(    clk=> clk,
             reset=> reset,
             en=> en,
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
    en<='0';
    data_in1<=(others => '0');
    data_in2<=(others => '0');
    data_in3<=(others => '0');
    
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"1111";
    data_in2<=x"2222";
    data_in3<=x"3333";
    en<='1';
    
    wait for 10ns;

    reset<='0';
    data_in1<=x"2222";
    data_in2<=x"3333";
    data_in3<=x"4444";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"3333";
    data_in2<=x"4444";
    data_in3<=x"5555";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"4444";
    data_in2<=x"5555";
    data_in3<=x"6666";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"5555";
    data_in2<=x"6666";
    data_in3<=x"7777";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"6666";
    data_in2<=x"7777";
    data_in3<=x"8888";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"7777";
    data_in2<=x"8888";
    data_in3<=x"9999";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"8888";
    data_in2<=x"9999";
    data_in3<=x"AAAA";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"9999";
    data_in2<=x"AAAA";
    data_in3<=x"BBBB";
    en<='1';
    wait for 10ns;
    
    reset<='0';
    data_in1<=x"AAAA";
    data_in2<=x"BBBB";
    data_in3<=x"CCCC";
    en<='0';
    wait for 10ns;
    reset<='0';
    data_in1<=x"BBBB";
    data_in2<=x"CCCC";
    data_in3<=x"DDDD";
    en<='1';
    wait for 10ns;
    en<='0';

    wait;
    
end process;

end Behavioral;
