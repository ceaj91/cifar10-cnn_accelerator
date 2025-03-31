for {set i 1} {$i < 4} {incr i} {
    set db_name "covdb_$i" ;
    set xsim_command "set_property -name \{xsim.simulate.xsim.more_options\} -value \{-testplusarg UVM_TESTNAME=test_simple$i -testplusarg UVM_VERBOSITY=UVM_LOW -sv_seed random -runall -cov_db_name $db_name\} -objects \[get_filesets sim_1\]"
    eval $xsim_command
    launch_simulation
    run all
    if {$i+1 < 5} {
        close_sim
    }
}