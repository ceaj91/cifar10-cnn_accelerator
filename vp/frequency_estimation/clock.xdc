create_clock -period 10.000 -name clk [get_ports clk]
set_input_delay -clock "clk" 5.0 [get_ports pic*]
set_output_delay -clock "clk" 5.0 [all_outputs]


