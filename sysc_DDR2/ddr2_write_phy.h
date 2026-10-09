#ifndef DDR2_WRITE_PHY_H_
#define DDR2_WRITE_PHY_H_

#include "systemc.h"
#include "ddr2_write_phy_pos.h"
#include "ddr2_write_phy_neg.h"

SC_MODULE(ddr2_write_phy)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<bool> phy_wr_start;
    sc_in<sc_uint<64> > phy_wr_data;
    sc_in<sc_uint<8> > phy_wr_mask;

    sc_out<sc_uint<16> > dq_out;
    sc_out<sc_uint<2> > dm_out;
    sc_out<sc_uint<2> > dqs_out;
    sc_out<bool> dq_oe;
    sc_out<bool> dqs_oe;
    sc_out<bool> phy_wr_busy;
    sc_out<bool> phy_wr_done;

    sc_signal<sc_uint<16> > dq_rise_sig;
    sc_signal<sc_uint<2> > dm_rise_sig;
    sc_signal<sc_uint<2> > dqs_rise_sig;

    sc_signal<sc_uint<16> > dq_fall_next_sig;
    sc_signal<sc_uint<2> > dm_fall_next_sig;
    sc_signal<sc_uint<2> > dqs_fall_next_sig;

    sc_signal<sc_uint<16> > dq_fall_sig;
    sc_signal<sc_uint<2> > dm_fall_sig;
    sc_signal<sc_uint<2> > dqs_fall_sig;

    ddr2_write_phy_pos *u_pos;
    ddr2_write_phy_neg *u_neg;

    void output_mux(void);

    SC_CTOR(ddr2_write_phy)
    {
        u_pos = new ddr2_write_phy_pos("u_pos");
        u_pos->clk(clk);
        u_pos->rst(rst);
        u_pos->phy_wr_start(phy_wr_start);
        u_pos->phy_wr_data(phy_wr_data);
        u_pos->phy_wr_mask(phy_wr_mask);
        u_pos->dq_rise(dq_rise_sig);
        u_pos->dm_rise(dm_rise_sig);
        u_pos->dqs_rise(dqs_rise_sig);
        u_pos->dq_fall_next(dq_fall_next_sig);
        u_pos->dm_fall_next(dm_fall_next_sig);
        u_pos->dqs_fall_next(dqs_fall_next_sig);
        u_pos->dq_oe(dq_oe);
        u_pos->dqs_oe(dqs_oe);
        u_pos->phy_wr_busy(phy_wr_busy);
        u_pos->phy_wr_done(phy_wr_done);

        u_neg = new ddr2_write_phy_neg("u_neg");
        u_neg->clk(clk);
        u_neg->rst(rst);
        u_neg->dq_in(dq_fall_next_sig);
        u_neg->dm_in(dm_fall_next_sig);
        u_neg->dqs_in(dqs_fall_next_sig);
        u_neg->dq_fall(dq_fall_sig);
        u_neg->dm_fall(dm_fall_sig);
        u_neg->dqs_fall(dqs_fall_sig);

        SC_METHOD(output_mux);
        sensitive << clk
                  << dq_rise_sig << dm_rise_sig << dqs_rise_sig
                  << dq_fall_sig << dm_fall_sig << dqs_fall_sig;
    }
};

#endif
