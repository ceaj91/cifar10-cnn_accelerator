

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity add_tree_tb is
generic(NUM_TO_ADD:integer := 9;
        WORD_SIZE: integer := 16);
--  Port ( );
end add_tree_tb;

architecture Behavioral of add_tree_tb is
signal inputs_to_add :  std_logic_vector(NUM_TO_ADD * WORD_SIZE - 1 downto 0);
signal bias :  std_logic_vector(WORD_SIZE-1 downto 0);
signal add_enable :  std_logic;
signal clk :  std_logic;
signal reset :  std_logic;

--out_valid: out std_logic;
signal output_pixel :  std_logic_vector(WORD_SIZE - 1 downto 0);
signal valid_out :  std_logic;
begin

duv: entity work.add_tree
generic map(NUM_TO_ADD=>NUM_TO_ADD,WORD_SIZE=>WORD_SIZE)
port map(
         inputs_to_add=>inputs_to_add ,
         bias=> bias,
         add_enable=>add_enable ,
         clk =>clk ,
         reset=>reset ,
         output_pixel=> output_pixel,
         valid_out=>valid_out);


clk_gen: process is begin
    clk <= '0';
    wait for 10ns;
    clk <= '1';
    wait for 10ns;
end process;

stim_gen: process is begin
reset <='1';
add_enable <='0';

wait for 25ns;
reset <='0';
add_enable <='1';
inputs_to_add(0*16 + 15 downto 0*16) <= std_logic_vector(to_unsigned(1,16));
inputs_to_add(1*16 + 15 downto 1*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(2*16 + 15 downto 2*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(3*16 + 15 downto 3*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(4*16 + 15 downto 4*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(5*16 + 15 downto 5*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(6*16 + 15 downto 6*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(7*16 + 15 downto 7*16)<= std_logic_vector(to_unsigned(1,16));
inputs_to_add(8*16 + 15 downto 8*16)<= std_logic_vector(to_unsigned(1,16));
bias <=std_logic_vector(to_unsigned(1,16));
wait for 10ns;
add_enable <='0';
inputs_to_add(0*16 + 15 downto 0*16) <= std_logic_vector(to_unsigned(2,16));
inputs_to_add(1*16 + 15 downto 1*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(2*16 + 15 downto 2*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(3*16 + 15 downto 3*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(4*16 + 15 downto 4*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(5*16 + 15 downto 5*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(6*16 + 15 downto 6*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(7*16 + 15 downto 7*16)<= std_logic_vector(to_unsigned(2,16));
inputs_to_add(8*16 + 15 downto 8*16)<= std_logic_vector(to_unsigned(2,16));
bias <=std_logic_vector(to_unsigned(2,16));
wait for 105ns;
add_enable <='1';
wait for 15ns;
add_enable <='0';
wait for 100ns;
reset<='1';
wait;
end process;
end Behavioral;
