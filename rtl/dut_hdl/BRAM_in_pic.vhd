library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity BRAM_in_pic is
generic (
        RAM_WIDTH : integer := 16;
        RAM_DEPTH : integer := 16384;
        ADDR_SIZE : integer := 14); --based on RAM_DEPTH   ADDR_SIZE = log2(RAM_DEPTH);
port (
    clk : in std_logic;
    en : in std_logic;
    we : in std_logic;
    addr_read1 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_read2 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_read3 : in std_logic_vector(ADDR_SIZE-1  downto 0);
    addr_write : in std_logic_vector(ADDR_SIZE-1  downto 0);
    data_in : in std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out1 : out std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out2 : out std_logic_vector(RAM_WIDTH - 1 downto 0);
    data_out3 : out std_logic_vector(RAM_WIDTH - 1 downto 0)
  );
end BRAM_in_pic;

architecture rtl of BRAM_in_pic is
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
        data_out1 <= memory(to_integer(unsigned(addr_read1)));
        data_out2 <= memory(to_integer(unsigned(addr_read2)));
        data_out3 <= memory(to_integer(unsigned(addr_read3)));
      end if;
    end if;
  end process;
end rtl;
