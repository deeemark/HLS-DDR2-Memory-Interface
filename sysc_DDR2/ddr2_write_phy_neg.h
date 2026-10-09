#ifndef DDR2_WRITE_PHY_NEG_H_
#define DDR2_WRITE_PHY_NEG_H_

#include "systemc.h"

SC_MODULE(ddr2_write_phy_neg)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<sc_uint<16> > dq_in;
    sc_in<sc_uint<2> > dm_in;
    sc_in<sc_uint<2> > dqs_in;

    sc_out<sc_uint<16> > dq_fall;
    sc_out<sc_uint<2> > dm_fall;
    sc_out<sc_uint<2> > dqs_fall;

    void neg_proc(void);

    SC_CTOR(ddr2_write_phy_neg)
    {
        SC_CTHREAD(neg_proc, clk.neg());
        reset_signal_is(rst, true);
    }
};

#endif
