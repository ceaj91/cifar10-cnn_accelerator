

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;


entity BRAM_in_pic_tb is
generic (
        RAM_WIDTH : integer := 16;
        RAM_DEPTH : integer := 16384;
        ADDR_SIZE : integer := 14);
--  Port ( );
end BRAM_in_pic_tb;

architecture Behavioral of BRAM_in_pic_tb is
signal    clk :  std_logic;
signal    en :  std_logic;
signal    we :  std_logic;
signal    addr_read1 :  std_logic_vector(ADDR_SIZE-1  downto 0);
signal    addr_read2 :  std_logic_vector(ADDR_SIZE-1  downto 0);
signal    addr_read3 :  std_logic_vector(ADDR_SIZE-1  downto 0);
signal    addr_write :  std_logic_vector(ADDR_SIZE-1  downto 0);
signal    data_in :  std_logic_vector(RAM_WIDTH - 1 downto 0);
signal    data_out1 :  std_logic_vector(RAM_WIDTH - 1 downto 0);
signal    data_out2 :  std_logic_vector(RAM_WIDTH - 1 downto 0);
signal    data_out3 :  std_logic_vector(RAM_WIDTH - 1 downto 0);

signal addr1: integer :=1;
signal addr2: integer :=2;
signal addr3: integer :=3;


begin

input_picture_mem: entity work.BRAM_in_pic 
    generic map(RAM_WIDTH=>RAM_WIDTH, RAM_DEPTH=>RAM_DEPTH, ADDR_SIZE=>ADDR_SIZE)
    port map(clk=>clk,
             en=>en, 
             we=>we,
             addr_read1=>addr_read1,
             addr_read2=>addr_read2,
             addr_read3=>addr_read3,
             addr_write=>addr_write,
             data_in=>data_in,
             data_out1=>data_out1,
             data_out2=>data_out2,
             data_out3=>data_out3);
 clk_gen: process is begin
    clk <= '0';
    wait for 5 ns;
    clk <= '1';
    wait for 5 ns;
end process;

stim_gen : process is begin
    en<='1';
    we<='1';
    
    addr_read1<=(others => '0');
    addr_read2<=(others => '0');
    addr_read3<=(others => '0');
    
    wait for 10ns;
    for i in 0 to 5000 loop
        addr_write <= std_logic_vector(to_unsigned(i,ADDR_SIZE));
        data_in <= std_logic_vector(to_unsigned(i,RAM_WIDTH));
        wait for 10ns;
    end loop;
    we<='0';
    
    wait for 10ns;
    for i in 0 to 4500 loop
        addr_read1<=std_logic_vector(to_unsigned(addr1,ADDR_SIZE));
        addr_read2<=std_logic_vector(to_unsigned(addr2,ADDR_SIZE));
        addr_read3<=std_logic_vector(to_unsigned(addr3,ADDR_SIZE));
        addr1 <= addr1+3;
        addr2 <= addr2+3;
        addr3 <= addr3+3;
        wait for 10ns;
    end loop;
    
    wait;
end process;            
end Behavioral;
