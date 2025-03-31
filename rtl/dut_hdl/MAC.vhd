
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity MAC_pipeline is
Port ( picture_param: in std_logic_vector(15 downto 0);
       weights_param: in std_logic_vector(15 downto 0);
       clk : in std_logic;
       reset : in std_logic;
       en:in std_logic;
       output: out std_logic_vector(15 downto 0)
       );
end MAC_pipeline;

architecture Behavioral of MAC_pipeline is
-- attribute use_dsp : string;
-- attribute use_dsp of Behavioral : architecture is "yes";

signal in_picture_reg : std_logic_vector(15 downto 0);
signal in_weights_reg : std_logic_vector(15 downto 0);
signal mlp_res_reg : std_logic_vector(31 downto 0);
signal acm_reg : std_logic_vector(31 downto 0);
--signal en_mlp_reg: std_logic;
--signal en_acm_reg: std_logic;

signal in_picture_next : std_logic_vector(15 downto 0);
signal in_weights_next : std_logic_vector(15 downto 0);
signal mlp_res_next : std_logic_vector(31 downto 0);
signal acm_next : std_logic_vector(31 downto 0);
--signal en_mlp_next: std_logic;
--signal en_acm_next: std_logic;
 --signal mlp_res_signal : std_logic_vector(31 downto 0);

begin

process(clk) 

begin

if(rising_edge(clk)) then
    --en_mlp_reg <= en_mlp_next;
    --en_acm_reg <= en_acm_next;
    
    
    if(reset = '1') then
        in_picture_reg <= (others => '0');
        in_weights_reg <= (others => '0');
        mlp_res_reg <= (others => '0');
        acm_reg <= (others => '0');
        --en_mlp_reg <= '0';
        --en_acm_reg <= '0';
     else
        if(en = '1') then
            in_picture_reg <= in_picture_next;
            in_weights_reg <= in_weights_next;
        --end if;
        --if(en_mlp_reg = '1') then
            mlp_res_reg <=mlp_res_next;
        --end if;
        --if(en_acm_reg = '1') then
            acm_reg <= acm_next;
        end if;
    end if;
end if;
end process;

in_picture_next<=picture_param;
in_weights_next<=weights_param;
--mlp_res_signal<= std_logic_vector(signed(in_picture_reg) * signed(in_weights_reg));
--mlp_res_next <= mlp_res_signal(27 downto 24) & mlp_res_signal(23 downto 12);
mlp_res_next <= std_logic_vector(signed(in_picture_reg) * signed(in_weights_reg));

acm_next <= std_logic_vector(signed(acm_reg) + signed(mlp_res_reg));
--en_mlp_next <= en;
--en_acm_next <= en_mlp_reg;
output <= acm_reg(27 downto 24) & acm_reg(23 downto 12);
end Behavioral;

