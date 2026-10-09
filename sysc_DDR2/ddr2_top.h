#ifndef DDR2_TOP_H_
#define DDR2_TOP_H_

#include <systemc.h>
#include "ddr2_controller.h"
#include "ddr2_write_phy.h"
#include "ddr2_read_phy.h"

SC_MODULE(ddr2_top)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<bool> req_valid;
    sc_in<bool> req_write;
    sc_in<sc_uint<24> > req_addr;
    sc_in<sc_uint<64> > req_wdata;
    sc_in<sc_uint<8> > req_wmask;

    sc_out<bool> req_ready;
    sc_out<bool> rsp_valid;
    sc_out<sc_uint<64> > rsp_rdata;

    sc_out<bool> ddr_cke;
    sc_out<bool> ddr_cs_n;
    sc_out<bool> ddr_ras_n;
    sc_out<bool> ddr_cas_n;
    sc_out<bool> ddr_we_n;
    sc_out<bool> ddr_odt;
    sc_out<sc_uint<2> > ddr_ba;
    sc_out<sc_uint<13> > ddr_addr;
    sc_out<bool> init_done;

    // Logical DDR PHY pins/boundary.
    sc_out<sc_uint<16> > dq_out;
    sc_in<sc_uint<16> > dq_in;
    sc_out<sc_uint<2> > dm_out;
    sc_out<sc_uint<2> > dqs_out;
    sc_out<bool> dq_oe;
    sc_out<bool> dqs_oe;

    sc_signal<bool> phy_wr_start_sig;
    sc_signal<sc_uint<64> > phy_wr_data_sig;
    sc_signal<sc_uint<8> > phy_wr_mask_sig;
    sc_signal<bool> phy_rd_start_sig;
    sc_signal<bool> phy_rd_valid_sig;
    sc_signal<sc_uint<64> > phy_rd_data_sig;
    sc_signal<bool> phy_wr_busy_sig;
    sc_signal<bool> phy_wr_done_sig;

    ddr2_controller *u_controller;
    ddr2_write_phy *u_write_phy;
    ddr2_read_phy *u_read_phy;

    SC_CTOR(ddr2_top)
    {
        u_controller = new ddr2_controller("u_controller");
        u_controller->clk(clk);
        u_controller->rst(rst);
        u_controller->req_valid(req_valid);
        u_controller->req_write(req_write);
        u_controller->req_addr(req_addr);
        u_controller->req_wdata(req_wdata);
        u_controller->req_wmask(req_wmask);
        u_controller->req_ready(req_ready);
        u_controller->rsp_valid(rsp_valid);
        u_controller->rsp_rdata(rsp_rdata);
        u_controller->ddr_cke(ddr_cke);
        u_controller->ddr_cs_n(ddr_cs_n);
        u_controller->ddr_ras_n(ddr_ras_n);
        u_controller->ddr_cas_n(ddr_cas_n);
        u_controller->ddr_we_n(ddr_we_n);
        u_controller->ddr_odt(ddr_odt);
        u_controller->ddr_ba(ddr_ba);
        u_controller->ddr_addr(ddr_addr);
        u_controller->init_done(init_done);
        u_controller->phy_wr_start(phy_wr_start_sig);
        u_controller->phy_wr_data(phy_wr_data_sig);
        u_controller->phy_wr_mask(phy_wr_mask_sig);
        u_controller->phy_rd_start(phy_rd_start_sig);
        u_controller->phy_rd_valid(phy_rd_valid_sig);
        u_controller->phy_rd_data(phy_rd_data_sig);

        u_write_phy = new ddr2_write_phy("u_write_phy");
        u_write_phy->clk(clk);
        u_write_phy->rst(rst);
        u_write_phy->phy_wr_start(phy_wr_start_sig);
        u_write_phy->phy_wr_data(phy_wr_data_sig);
        u_write_phy->phy_wr_mask(phy_wr_mask_sig);
        u_write_phy->dq_out(dq_out);
        u_write_phy->dm_out(dm_out);
        u_write_phy->dqs_out(dqs_out);
        u_write_phy->dq_oe(dq_oe);
        u_write_phy->dqs_oe(dqs_oe);
        u_write_phy->phy_wr_busy(phy_wr_busy_sig);
        u_write_phy->phy_wr_done(phy_wr_done_sig);

        u_read_phy = new ddr2_read_phy("u_read_phy");
        u_read_phy->clk(clk);
        u_read_phy->rst(rst);
        u_read_phy->phy_rd_start(phy_rd_start_sig);
        u_read_phy->dq_in(dq_in);
        u_read_phy->phy_rd_data(phy_rd_data_sig);
        u_read_phy->phy_rd_valid(phy_rd_valid_sig);
    }
};

#endif
