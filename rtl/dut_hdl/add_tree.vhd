

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;

entity add_tree is
generic(NUM_TO_ADD:integer := 9;
        WORD_SIZE: integer := 16);
Port (inputs_to_add : in std_logic_vector(NUM_TO_ADD * WORD_SIZE - 1 downto 0);
      bias : in std_logic_vector(WORD_SIZE-1 downto 0);
      add_enable : in std_logic;
      clk : in std_logic;
      reset : in std_logic;
      
      --out_valid: out std_logic;
      output_pixel : out std_logic_vector(WORD_SIZE - 1 downto 0);
      valid_out : out std_logic);
end add_tree;

architecture Behavioral of add_tree is
attribute use_dsp : string;
 attribute use_dsp of Behavioral : architecture is "yes";
type reg_map is array (0 to 19) of std_logic_vector(WORD_SIZE-1 downto 0);

signal reg_reg:reg_map;
signal reg_next:reg_map;

signal bias_next : std_logic_vector(WORD_SIZE-1 downto 0);
signal bias_reg : std_logic_vector(WORD_SIZE-1 downto 0);

signal en_stage1_reg:std_logic;
signal en_stage1_next:std_logic;

signal en_stage2_reg:std_logic;
signal en_stage2_next:std_logic;

signal en_stage3_reg:std_logic;
signal en_stage3_next:std_logic;

signal en_stage4_reg:std_logic;
signal en_stage4_next:std_logic;

signal en_stage5_reg:std_logic;
signal en_stage5_next:std_logic;

begin

process(clk) begin
    if(rising_edge(clk)) then
        en_stage1_reg <= en_stage1_next;
        en_stage2_reg <= en_stage2_next;
        en_stage3_reg <= en_stage3_next;
        en_stage4_reg <= en_stage4_next;
        en_stage5_reg <= en_stage5_next;
        if(reset = '1') then
            for i in 0 to 19 loop
                reg_reg(i) <= (others => '0');
            end loop;
            en_stage1_reg <= '0';
            en_stage2_reg <= '0';
            en_stage3_reg <= '0';
            en_stage4_reg <= '0';
            en_stage5_reg <= '0';
            bias_reg <= (others=>'0');
        else
            if(add_enable = '1') then
                bias_reg<=bias_next;
                for i in 0 to 8 loop
                    reg_reg(i) <= reg_next(i);
                end loop;
            end if;
            if(en_stage1_reg = '1') then
                for i in 9 to 13 loop
                    reg_reg(i) <= reg_next(i);
                end loop;
            end if;
            if(en_stage2_reg = '1') then
                for i in 14 to 16 loop
                    reg_reg(i) <= reg_next(i);
                end loop;
            end if;
            if(en_stage3_reg = '1') then
                for i in 17 to 18 loop
                    reg_reg(i) <= reg_next(i);
                end loop;
            end if;
            if(en_stage4_reg = '1') then
                    reg_reg(19) <= reg_next(19);
            end if;
            
        end if;
    end if;

end process;


en_stage1_next<=add_enable;
en_stage2_next<=en_stage1_reg;
en_stage3_next<=en_stage2_reg;
en_stage4_next<=en_stage3_reg;
en_stage5_next<=en_stage4_reg;


reg_next(0) <= inputs_to_add(0*16 + 15 downto 0*16);
reg_next(1) <= inputs_to_add(1*16 + 15 downto 1*16);
reg_next(2) <= inputs_to_add(2*16 + 15 downto 2*16);
reg_next(3) <= inputs_to_add(3*16 + 15 downto 3*16);
reg_next(4) <= inputs_to_add(4*16 + 15 downto 4*16);
reg_next(5) <= inputs_to_add(5*16 + 15 downto 5*16);
reg_next(6) <= inputs_to_add(6*16 + 15 downto 6*16);
reg_next(7) <= inputs_to_add(7*16 + 15 downto 7*16);
reg_next(8) <= inputs_to_add(8*16 + 15 downto 8*16);
bias_next <= bias;

reg_next(9) <= std_logic_vector(signed(reg_reg(0)) + signed(reg_reg(1)));
reg_next(10) <= std_logic_vector(signed(reg_reg(2)) + signed(reg_reg(3)));
reg_next(11) <= std_logic_vector(signed(reg_reg(4)) + signed(reg_reg(5)));
reg_next(12) <= std_logic_vector(signed(reg_reg(6)) + signed(reg_reg(7)));
reg_next(13) <= std_logic_vector(signed(reg_reg(8)) + signed(bias_reg));

reg_next(14) <= std_logic_vector(signed(reg_reg(9)) + signed(reg_reg(10)));
reg_next(15) <= std_logic_vector(signed(reg_reg(11)) + signed(reg_reg(12)));
reg_next(16) <= reg_reg(13);

reg_next(17) <= std_logic_vector(signed(reg_reg(14)) + signed(reg_reg(15)));
reg_next(18) <= reg_reg(16);

reg_next(19) <= std_logic_vector(signed(reg_reg(17)) + signed(reg_reg(18)));


output_pixel <= reg_reg(19);
valid_out<=en_stage5_reg;

end Behavioral;
