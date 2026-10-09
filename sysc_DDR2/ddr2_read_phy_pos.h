#ifndef DDR2_READ_PHY_POS_H_
#define DDR2_READ_PHY_POS_H_
#include "systemc.h"

SC_MODULE(ddr2_read_phy_pos)
{
    sc_in_clk clk;
    sc_in<bool> rst;
    sc_in<bool> phy_rd_start;
    sc_in<sc_uint<16> > dq_in;
    sc_out<sc_uint<16> > beat1;
    sc_out<sc_uint<16> > beat3;

    void pos_proc(void);

    SC_CTOR(ddr2_read_phy_pos)
    {
        SC_CTHREAD(pos_proc, clk.pos());
        reset_signal_is(rst, true);
    }
};
#endif
