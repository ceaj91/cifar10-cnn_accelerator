#ifndef DMA_H
#define DMA_H
#define SC_INCLUDE_FX

#include <systemc>
#include "types.hpp"
#include "tlm_utils/tlm_quantumkeeper.h"
#include <string>
#include <array>
#include <vector>
using namespace std;
using namespace sc_core;
using namespace tlm;

SC_MODULE(DMA)
{
	public:
		SC_HAS_PROCESS(DMA);
		DMA(sc_module_name name);
		tlm_utils::simple_target_socket<DMA> s_dma_t;
		tlm_utils::simple_initiator_socket<DMA> s_dma_i0;

		sc_port<sc_fifo_out_if<hwdata_t>> p_fifo_out;
		sc_port<sc_fifo_in_if<hwdata_t>> p_fifo_in;
	protected:
		void b_transport(pl_t&, sc_time&);
		void send_to_fifo();
		void read_from_fifo();
		sc_logic send;
		sc_logic read;
		vector<hwdata_t> tmp_mem;

		tlm_command cmd;
		uint64 adr;
		unsigned int length;
		unsigned char *buf;
		unsigned int start;
};
#endif
