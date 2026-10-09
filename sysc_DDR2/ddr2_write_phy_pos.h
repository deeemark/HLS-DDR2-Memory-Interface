#ifndef DDR2_WRITE_PHY_POS_H_
#define DDR2_WRITE_PHY_POS_H_

#include "systemc.h"

SC_MODULE(ddr2_write_phy_pos)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<bool> phy_wr_start;
    sc_in<sc_uint<64> > phy_wr_data;
    sc_in<sc_uint<8> > phy_wr_mask;

    sc_out<sc_uint<16> > dq_rise;
    sc_out<sc_uint<2> > dm_rise;
    sc_out<sc_uint<2> > dqs_rise;

    sc_out<sc_uint<16> > dq_fall_next;
    sc_out<sc_uint<2> > dm_fall_next;
    sc_out<sc_uint<2> > dqs_fall_next;

    sc_out<bool> dq_oe;
    sc_out<bool> dqs_oe;
    sc_out<bool> phy_wr_busy;
    sc_out<bool> phy_wr_done;

    void pos_proc(void);

    SC_CTOR(ddr2_write_phy_pos)
    {
        SC_CTHREAD(pos_proc, clk.pos());
        reset_signal_is(rst, true);
    }
};

#endif
