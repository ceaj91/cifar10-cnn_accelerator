
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity MAC_top is
generic (
        NUM_MACs : integer := 9;
        WORD_SIZE: integer := 16);
Port (    picture_params : in std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
          weights_params : in std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0);
          clk : in std_logic;
          reset : in std_logic;
          en : in std_logic;
          outputs : out std_logic_vector((NUM_MACs * WORD_SIZE) - 1 downto 0));    
end MAC_top;

architecture Behavioral of MAC_top is

component MAC_pipeline 
        Port (
            picture_param : in std_logic_vector(WORD_SIZE-1 downto 0);
            weights_param : in std_logic_vector(WORD_SIZE-1 downto 0);
            clk : in std_logic;
            reset : in std_logic;
            en : in std_logic;
            output : out std_logic_vector(WORD_SIZE-1 downto 0)
        );
    end component;
    

 
 
begin

    

    
 
        
    gen_MACs: for i in 0 to NUM_MACs-1 generate
    MAC_inst: MAC_pipeline
        Port map (
            picture_param => picture_params(i*WORD_SIZE+(WORD_SIZE-1) downto i*WORD_SIZE),
            weights_param => weights_params(i*WORD_SIZE+(WORD_SIZE-1) downto i*WORD_SIZE),
            clk => clk,
            reset => reset,
            en => en,
            output => outputs(i*WORD_SIZE+(WORD_SIZE-1) downto i*WORD_SIZE)
            );
    end generate;
    

end Behavioral;
