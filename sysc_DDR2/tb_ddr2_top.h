#ifndef TB_DDR2_TOP_H_
#define TB_DDR2_TOP_H_

#include <systemc.h>

SC_MODULE(tb_ddr2_top)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_out<bool> req_valid;
    sc_out<bool> req_write;
    sc_out<sc_uint<24> > req_addr;
    sc_out<sc_uint<64> > req_wdata;
    sc_out<sc_uint<8> > req_wmask;
    sc_in<bool> req_ready;
    sc_in<bool> rsp_valid;
    sc_in<sc_uint<64> > rsp_rdata;

    sc_in<bool> ddr_cs_n;
    sc_in<bool> ddr_ras_n;
    sc_in<bool> ddr_cas_n;
    sc_in<bool> ddr_we_n;
    sc_in<bool> init_done;

    sc_in<sc_uint<16> > dq_out;
    sc_out<sc_uint<16> > dq_in;
    sc_in<sc_uint<2> > dm_out;
    sc_in<bool> dq_oe;

    bool failed;

    void run();

    SC_CTOR(tb_ddr2_top) : failed(false)
    {
        SC_THREAD(run);
    }
};

#endif
