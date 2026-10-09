#ifndef DDR2_READ_PHY_H_
#define DDR2_READ_PHY_H_

#include "systemc.h"
#include "ddr2_read_phy_pos.h"
#include "ddr2_read_phy_neg.h"

SC_MODULE(ddr2_read_phy)
{
    sc_in_clk clk;
    sc_in<bool> rst;
    sc_in<bool> phy_rd_start;
    sc_in<sc_uint<16> > dq_in;

    sc_out<sc_uint<64> > phy_rd_data;
    sc_out<bool> phy_rd_valid;

    sc_signal<sc_uint<16> > beat0_sig;
    sc_signal<sc_uint<16> > beat1_sig;
    sc_signal<sc_uint<16> > beat2_sig;
    sc_signal<sc_uint<16> > beat3_sig;

    ddr2_read_phy_pos *u_pos;
    ddr2_read_phy_neg *u_neg;

    void control_proc(void);

    SC_CTOR(ddr2_read_phy)
    {
        u_pos = new ddr2_read_phy_pos("u_pos");
        u_pos->clk(clk);
        u_pos->rst(rst);
        u_pos->phy_rd_start(phy_rd_start);
        u_pos->dq_in(dq_in);
        u_pos->beat1(beat1_sig);
        u_pos->beat3(beat3_sig);

        u_neg = new ddr2_read_phy_neg("u_neg");
        u_neg->clk(clk);
        u_neg->rst(rst);
        u_neg->phy_rd_start(phy_rd_start);
        u_neg->dq_in(dq_in);
        u_neg->beat0(beat0_sig);
        u_neg->beat2(beat2_sig);

        SC_CTHREAD(control_proc, clk.pos());
        reset_signal_is(rst, true);
    }
};
#endif
