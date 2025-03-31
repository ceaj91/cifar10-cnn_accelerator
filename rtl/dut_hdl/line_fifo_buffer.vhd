
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity line_fifo_buffer is
generic(WORD_SIZE: integer := 16;
        LINE_LENGHT:integer := 32);
Port (clk:in std_logic;
      reset:in std_logic;
      en_line: in std_logic;
      data_in: in std_logic_vector(WORD_SIZE-1 downto 0); 
      data_out: out std_logic_vector(WORD_SIZE-1 downto 0));
end line_fifo_buffer;
architecture Behavioral of line_fifo_buffer is

type line_buffer_type is array (0 to LINE_LENGHT - 1) of std_logic_vector(WORD_SIZE - 1 downto 0);
 
signal element_next : line_buffer_type;
signal element_reg : line_buffer_type;
begin

process(clk) begin
    
        if(rising_edge(clk)) then  
            if(reset = '1') then
                for i in 0 to LINE_LENGHT - 1 loop
                    element_reg(i) <= (others =>'0');
                end loop;
            elsif(en_line='1') then
                for i in 0 to LINE_LENGHT - 1 loop
                    element_reg(i) <= element_next(i);
                end loop;
            end if;
        
        end if;
 
end process;

element_next(0) <= data_in;
gen_loop: for i in 1 to LINE_LENGHT - 1 generate
    element_next(i) <=  element_reg(i-1);
end generate;


data_out <= element_reg(LINE_LENGHT-1);
      
   

end Behavioral;
