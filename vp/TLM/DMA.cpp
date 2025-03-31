#ifndef DMA_C
#define DMA_C
#include "DMA.hpp"

DMA::DMA(sc_module_name name):sc_module(name)
{
   s_dma_t.register_b_transport(this, &DMA::b_transport);
   //cout<<name<<" constructed"<<endl;
   SC_THREAD(send_to_fifo);
   SC_THREAD(read_from_fifo);
   //tmp_mem.reserve();
   send = SC_LOGIC_0;
   read = SC_LOGIC_0;
}

void DMA::b_transport(pl_t& pl, sc_time& offset)
{
	cmd         = pl.get_command();
	adr         = pl.get_address();
	length      = pl.get_data_length();
	buf         = pl.get_data_ptr();
	start       = adr-0x81000000;
	switch (cmd)
	{
		case TLM_WRITE_COMMAND:
			//TAKE REQUESTED DATA FROM DDR3 RAM
			pl.set_command(TLM_READ_COMMAND);
			pl.set_address(start);
			s_dma_i0->b_transport(pl, offset);
			assert(pl.get_response_status() == TLM_OK_RESPONSE);

			//PUT IT IN TEMPORARY VARIABLE
			buf = pl.get_data_ptr();
			tmp_mem.clear();

			for(unsigned int i=0; i<length; i++)
			{
				tmp_mem.push_back(((hwdata_t*)buf)[i]);
			}

			//START SENDING IT INTO FIFO
			send = SC_LOGIC_1;
			//FINISH TRANSACTION

			//AFTER ITS DONE, FINISH TRANSACTION
			pl.set_response_status( TLM_OK_RESPONSE );
			offset += sc_time(20, SC_NS);
			break;

		case TLM_READ_COMMAND:
			//START READ FROM FIFO, OUTPUT IMAGE FROM CONV LAYER
			read = SC_LOGIC_1;
			offset += sc_time(20, SC_NS);
			break;

		default:
			pl.set_response_status( TLM_COMMAND_ERROR_RESPONSE );
	}
}

void DMA::send_to_fifo()
{
	sc_time offset=SC_ZERO_TIME;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	while(1)
	{
		while(send==SC_LOGIC_0)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(10, SC_NS));
			offset = qk.get_local_time();
			#else
			offset += sc_time(10, SC_NS);
			#endif

			#ifdef QUANTUM
			qk.set_and_sync(offset);
			#endif
		}

		send=SC_LOGIC_0;
		//PUSH IT INTO FIFO
		for(unsigned int i=0; i<length; i++)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(10, SC_NS));
			offset = qk.get_local_time();
			qk.set_and_sync(offset);
			#else
			offset += sc_time(10, SC_NS);
			#endif

			while(!p_fifo_out->nb_write(tmp_mem[i]))
			{
				#ifdef QUANTUM
				qk.inc(sc_time(10, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(10, SC_NS);
				#endif
			}

		}

	}

}

void DMA::read_from_fifo()
{
	pl_t pl;
	sc_time offset=SC_ZERO_TIME;
	hwdata_t fifo_read;
	#ifdef QUANTUM
	tlm_utils::tlm_quantumkeeper qk;
	qk.reset();
	#endif
	while(1)
	{
		while(read==SC_LOGIC_0)
		{
			#ifdef QUANTUM
			qk.inc(sc_time(10, SC_NS));
			offset = qk.get_local_time();
			#else
			offset += sc_time(10, SC_NS);
			#endif

			#ifdef QUANTUM
			qk.set_and_sync(offset);
			#endif
		}
		tmp_mem.clear();
		read=SC_LOGIC_0;

		//PUSH IT INTO FIFO TOWARDS SVM
		for (unsigned int i = 0; i < length; ++i)
		{
			while(!p_fifo_in->nb_read(fifo_read))
			{
				#ifdef QUANTUM
				qk.inc(sc_time(10, SC_NS));
				offset = qk.get_local_time();
				qk.set_and_sync(offset);
				#else
				offset += sc_time(10, SC_NS);
				#endif
			}
			tmp_mem.push_back(fifo_read);
		}
		//WHEN OUTPUT IMAGE IS LOADED, SENT IT TO DDR, START ADDRESS IS ADDRESS FROM CPU
		buf = (unsigned char *)&tmp_mem[0];
		pl.set_address(start);
		pl.set_data_length(length);
		pl.set_command(TLM_WRITE_COMMAND);
		pl.set_data_ptr(buf);
		s_dma_i0->b_transport(pl, offset);
		qk.set_and_sync(offset);
	}

}





#endif
