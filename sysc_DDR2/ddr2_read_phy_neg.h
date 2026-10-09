#ifndef DDR2_READ_PHY_NEG_H_
#define DDR2_READ_PHY_NEG_H_
#include "systemc.h"

SC_MODULE(ddr2_read_phy_neg)
{
    sc_in_clk clk;
    sc_in<bool> rst;
    sc_in<bool> phy_rd_start;
    sc_in<sc_uint<16> > dq_in;
    sc_out<sc_uint<16> > beat0;
    sc_out<sc_uint<16> > beat2;

    void neg_proc(void);

    SC_CTOR(ddr2_read_phy_neg)
    {
        SC_CTHREAD(neg_proc, clk.neg());
        reset_signal_is(rst, true);
    }
};
#endif
