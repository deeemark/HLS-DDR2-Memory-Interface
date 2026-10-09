#ifndef DDR2_CONTROLLER_SYSTEMC_H
#define DDR2_CONTROLLER_SYSTEMC_H
#include <systemc.h>

SC_MODULE(ddr2_controller) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> req_valid;
    sc_in<bool> req_write;
    sc_in<sc_uint<24> > req_addr;
    sc_in<sc_uint<64> > req_wdata;
    sc_in<sc_uint<8> > req_wmask;
    sc_out<bool> req_ready;
    sc_out<bool> rsp_valid;
    sc_out<sc_uint<64> > rsp_rdata;
    sc_out<bool> ddr_cke, ddr_cs_n, ddr_ras_n, ddr_cas_n, ddr_we_n, ddr_odt;
    sc_out<sc_uint<2> > ddr_ba;
    sc_out<sc_uint<13> > ddr_addr;
    sc_out<bool> init_done;
    sc_out<bool> phy_wr_start;
    sc_out<sc_uint<64> > phy_wr_data;
    sc_out<sc_uint<8> > phy_wr_mask;
    sc_out<bool> phy_rd_start;
    sc_in<bool> phy_rd_valid;
    sc_in<sc_uint<64> > phy_rd_data;

    sc_uint<5> state;
    sc_uint<16> wait_count;
    sc_uint<9> dll_count;
    sc_uint<16> refresh_count;
    sc_uint<64> latched_rdata;
    bool latched_write;
    sc_uint<24> latched_addr;
    sc_uint<64> latched_wdata;
    sc_uint<8> latched_wmask;

    void run();
    SC_CTOR(ddr2_controller) { SC_CTHREAD(run, clk.pos()); }
};
#endif
