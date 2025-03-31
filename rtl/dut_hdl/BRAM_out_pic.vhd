library ieee;                                          
use ieee.std_logic_1164.all;                           
use ieee.numeric_std.all;                              
                                                       

entity BRAM_out_pic is
generic (                           
      RAM_WIDTH : integer := 16;    
      RAM_DEPTH : integer := 32768;
      ADDR_SIZE : integer := 15);--based on RAM_DEPTH   ADDR_SIZE = log2(RAM_DEPTH);
Port (  clk : in std_logic;                                     
 en : in std_logic;                                      
 we : in std_logic;                                      
 addr_read : in std_logic_vector(ADDR_SIZE-1  downto 0);                  
 addr_write : in std_logic_vector(ADDR_SIZE-1  downto 0);         
 data_in : in std_logic_vector(RAM_WIDTH - 1 downto 0);  
 data_out : out std_logic_vector(RAM_WIDTH - 1 downto 0));
end BRAM_out_pic;

architecture Behavioral of BRAM_out_pic is
 type ram_type is array (0 to RAM_DEPTH - 1) of std_logic_vector(RAM_WIDTH - 1 downto 0);
  signal memory : ram_type;
begin
process (clk)
  begin
    if rising_edge(clk) then
      if en = '1' then
        if we = '1' then
          memory(to_integer(unsigned(addr_write))) <= data_in;
        end if;
        data_out <= memory(to_integer(unsigned(addr_read)));
      end if;
    end if;
  end process;

end Behavioral;
