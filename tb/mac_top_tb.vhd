library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity mac_top_tb is
generic (
        NUM_MACs : integer := 9;
        WORD_SIZE: integer := 16);
--  Port ( );
end mac_top_tb;

architecture Behavioral of mac_top_tb is

signal picture_params_s : std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
signal weights_params_s :  std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
signal clk_s: std_logic;
signal reset_s: std_logic;
signal en_s:std_logic;
signal outputs_s:std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0); 


signal input_piture_param1 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param2 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param3 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param4 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param5 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param6 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param7 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param8 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_piture_param9 : std_logic_vector(WORD_SIZE-1 downto 0);
  
 signal input_weight_param1 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param2 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param3 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param4 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param5 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param6 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param7 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param8 : std_logic_vector(WORD_SIZE-1 downto 0);
 signal input_weight_param9 : std_logic_vector(WORD_SIZE-1 downto 0);



begin
picture_params_s(0*16 + 15 downto 0*16) <= input_piture_param1;
picture_params_s(1*16 + 15 downto 1*16) <= input_piture_param2;
picture_params_s(2*16 + 15 downto 2*16) <= input_piture_param3;
picture_params_s(3*16 + 15 downto 3*16) <= input_piture_param4;
picture_params_s(4*16 + 15 downto 4*16) <= input_piture_param5;
picture_params_s(5*16 + 15 downto 5*16) <= input_piture_param6;
picture_params_s(6*16 + 15 downto 6*16) <= input_piture_param7;
picture_params_s(7*16 + 15 downto 7*16) <= input_piture_param8;
picture_params_s(8*16 + 15 downto 8*16) <= input_piture_param9;

weights_params_s(0*16 + 15 downto 0*16) <= input_weight_param1;
weights_params_s(1*16 + 15 downto 1*16) <= input_weight_param2;
weights_params_s(2*16 + 15 downto 2*16) <= input_weight_param3;
weights_params_s(3*16 + 15 downto 3*16) <= input_weight_param4;
weights_params_s(4*16 + 15 downto 4*16) <= input_weight_param5;
weights_params_s(5*16 + 15 downto 5*16) <= input_weight_param6;
weights_params_s(6*16 + 15 downto 6*16) <= input_weight_param7;
weights_params_s(7*16 + 15 downto 7*16) <= input_weight_param8;
weights_params_s(8*16 + 15 downto 8*16) <= input_weight_param9;

duv: entity work.MAC_top
    generic map(NUM_MACs=>NUM_MACs, WORD_SIZE=>WORD_SIZE)
    port map(picture_params=>picture_params_s,
             weights_params=>weights_params_s,
             clk=>clk_s,
             reset=>reset_s,
             en=>en_s,
             outputs=>outputs_s);
             
             
clk_gen:process is 
begin
    clk_s<='0';
    wait for 5ns;
    clk_s<='1';
    wait for 5ns;
 
end process;

stim_gen: process is begin
reset_s <='1';
en_s<='0';
input_piture_param1 <= x"0000";
input_piture_param2 <= x"0000";
input_piture_param3 <= x"0000";
input_piture_param4 <= x"0000";
input_piture_param5 <= x"0000";
input_piture_param6 <= x"0000";
input_piture_param7 <= x"0000";
input_piture_param8 <= x"0000";
input_piture_param9 <= x"0000";

input_weight_param1 <= x"0000";
input_weight_param2 <= x"0000";
input_weight_param3 <= x"0000";
input_weight_param4 <= x"0000";
input_weight_param5 <= x"0000";
input_weight_param6 <= x"0000";
input_weight_param7 <= x"0000";
input_weight_param8 <= x"0000";
input_weight_param9 <= x"0000";

wait for 20ns;
reset_s <='0';
en_s<='1';
input_piture_param1 <= x"0001";
input_piture_param2 <= x"0002";
input_piture_param3 <= x"0003";
input_piture_param4 <= x"0004";
input_piture_param5 <= x"0005";
input_piture_param6 <= x"0006";
input_piture_param7 <= x"0007";
input_piture_param8 <= x"0008";
input_piture_param9 <= x"0009";

input_weight_param1 <= x"0001";
input_weight_param2 <= x"0002";
input_weight_param3 <= x"0003";
input_weight_param4 <= x"0004";
input_weight_param5 <= x"0005";
input_weight_param6 <= x"0006";
input_weight_param7 <= x"0007";
input_weight_param8 <= x"0008";
input_weight_param9 <= x"0009";
wait for 10ns;
input_piture_param1 <= x"0002";
input_piture_param2 <= x"0003";
input_piture_param3 <= x"0004";
input_piture_param4 <= x"0005";
input_piture_param5 <= x"0006";
input_piture_param6 <= x"0007";
input_piture_param7 <= x"0008";
input_piture_param8 <= x"0009";
input_piture_param9 <= x"000A";

input_weight_param1 <= x"000B";
input_weight_param2 <= x"000C";
input_weight_param3 <= x"000D";
input_weight_param4 <= x"000E";
input_weight_param5 <= x"000F";
input_weight_param6 <= x"000A";
input_weight_param7 <= x"0003";
input_weight_param8 <= x"0002";
input_weight_param9 <= x"0004";
wait for 100ns;
en_s<='0';
wait for 20ns;
reset_s <='1';
wait;
end process;

end Behavioral;
