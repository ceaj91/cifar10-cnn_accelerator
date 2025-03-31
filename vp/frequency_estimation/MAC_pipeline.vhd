
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity MAC_pipeline is
Port ( picture_param: in std_logic_vector(15 downto 0);
       weights_param: in std_logic_vector(15 downto 0);
       clk : in std_logic;
       reset : in std_logic;
       output: out std_logic_vector(31 downto 0));
end MAC_pipeline;

architecture Behavioral of MAC_pipeline is

-- attribute use_dsp : string;
-- attribute use_dsp of Behavioral : architecture is "yes";

signal in_picture_reg : std_logic_vector(15 downto 0);
signal in_weights_reg : std_logic_vector(15 downto 0);
signal mlp_res_reg : std_logic_vector(31 downto 0);
signal acm_reg : std_logic_vector(31 downto 0);

signal in_picture_next : std_logic_vector(15 downto 0);
signal in_weights_next : std_logic_vector(15 downto 0);
signal mlp_res_next : std_logic_vector(31 downto 0);
signal acm_next : std_logic_vector(31 downto 0);
 
begin

process(clk) is
begin

if(rising_edge(clk)) then
    if(reset = '1') then
        in_picture_reg <= (others => '0');
        in_weights_reg <= (others => '0');
        mlp_res_reg <= (others => '0');
        acm_reg <= (others => '0');
    else
        in_picture_reg <= in_picture_next;
        in_weights_reg <= in_weights_next;
        mlp_res_reg <= mlp_res_next;
        acm_reg <= acm_next;
    end if;
end if;
end process;

in_picture_next<=picture_param;
in_weights_next<=weights_param;
mlp_res_next <= std_logic_vector(unsigned(in_picture_reg) * unsigned(in_weights_reg));
acm_next <= std_logic_vector(unsigned(acm_reg) + unsigned(mlp_res_reg));


output <= acm_reg;
end Behavioral;
