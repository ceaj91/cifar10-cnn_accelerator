#ifndef INT_CON_H
#define INT_CON_H
#define SC_INCLUDE_FX

#include <systemc>
#include <iostream>
#include <string>
#include "types.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
using namespace sc_core;

SC_MODULE(InterCon)
{
        public:
                InterCon(sc_module_name name);
                tlm_utils::simple_target_socket<InterCon> s_ic_t;
                tlm_utils::simple_initiator_socket<InterCon> s_ic_i0;
                tlm_utils::simple_initiator_socket<InterCon> s_ic_i1;


        protected:
                void b_transport(pl_t&, sc_core::sc_time&);
};


#endif
