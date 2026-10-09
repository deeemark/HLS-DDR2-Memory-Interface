// Manual compile example:
// g++ -std=gnu++98 -D_GLIBCXX_USE_CXX11_ABI=0 -O1 \
//   -I/proj/cad/cwb-6.1/osci/include -I. \
//   main.cpp tb_ddr2_top.cpp ddr2_controller.cpp \
//   ddr2_write_phy_pos.cpp ddr2_write_phy_neg.cpp ddr2_write_phy.cpp \
//   ddr2_read_phy_pos.cpp ddr2_read_phy_neg.cpp ddr2_read_phy.cpp \
//   ddr2_top.cpp -L/proj/cad/cwb-6.1/osci/lib-linux64 -lsystemc -lm \
//   -o ddr2_top_test

#include <systemc.h>
#include "ddr2_top.h"
#include "tb_ddr2_top.h"

int sc_main(int argc, char **argv)
{
    sc_clock clk("clk", 5, SC_NS, 0.5, 0, SC_NS, true);

    sc_signal<bool> rst;
    sc_signal<bool> req_valid, req_write, req_ready, rsp_valid;
    sc_signal<sc_uint<24> > req_addr;
    sc_signal<sc_uint<64> > req_wdata, rsp_rdata;
    sc_signal<sc_uint<8> > req_wmask;
    sc_signal<bool> ddr_cke, ddr_cs_n, ddr_ras_n, ddr_cas_n, ddr_we_n, ddr_odt;
    sc_signal<sc_uint<2> > ddr_ba;
    sc_signal<sc_uint<13> > ddr_addr;
    sc_signal<bool> init_done;
    sc_signal<sc_uint<16> > dq_out, dq_in;
    sc_signal<sc_uint<2> > dm_out, dqs_out;
    sc_signal<bool> dq_oe, dqs_oe;

    ddr2_top dut("dut");
    tb_ddr2_top test("test_ddr2_top");

    // Design under test.
    dut.clk(clk); dut.rst(rst);
    dut.req_valid(req_valid); dut.req_write(req_write); dut.req_addr(req_addr);
    dut.req_wdata(req_wdata); dut.req_wmask(req_wmask);
    dut.req_ready(req_ready); dut.rsp_valid(rsp_valid); dut.rsp_rdata(rsp_rdata);
    dut.ddr_cke(ddr_cke); dut.ddr_cs_n(ddr_cs_n); dut.ddr_ras_n(ddr_ras_n);
    dut.ddr_cas_n(ddr_cas_n); dut.ddr_we_n(ddr_we_n); dut.ddr_odt(ddr_odt);
    dut.ddr_ba(ddr_ba); dut.ddr_addr(ddr_addr); dut.init_done(init_done);
    dut.dq_out(dq_out); dut.dq_in(dq_in); dut.dm_out(dm_out); dut.dqs_out(dqs_out);
    dut.dq_oe(dq_oe); dut.dqs_oe(dqs_oe);

    // Testbench.
    test.clk(clk); test.rst(rst);
    test.req_valid(req_valid); test.req_write(req_write); test.req_addr(req_addr);
    test.req_wdata(req_wdata); test.req_wmask(req_wmask);
    test.req_ready(req_ready); test.rsp_valid(rsp_valid); test.rsp_rdata(rsp_rdata);
    test.ddr_cs_n(ddr_cs_n); test.ddr_ras_n(ddr_ras_n);
    test.ddr_cas_n(ddr_cas_n); test.ddr_we_n(ddr_we_n); test.init_done(init_done);
    test.dq_out(dq_out); test.dq_in(dq_in); test.dm_out(dm_out); test.dq_oe(dq_oe);

#ifdef WAVE_DUMP
    sc_trace_file *trace_file = sc_create_vcd_trace_file("trace_behav");
    sc_trace(trace_file, clk, "clk"); sc_trace(trace_file, rst, "rst");
    sc_trace(trace_file, req_valid, "req_valid"); sc_trace(trace_file, req_write, "req_write");
    sc_trace(trace_file, req_addr, "req_addr"); sc_trace(trace_file, req_wdata, "req_wdata");
    sc_trace(trace_file, req_wmask, "req_wmask"); sc_trace(trace_file, req_ready, "req_ready");
    sc_trace(trace_file, rsp_valid, "rsp_valid"); sc_trace(trace_file, rsp_rdata, "rsp_rdata");
    sc_trace(trace_file, init_done, "init_done");
    sc_trace(trace_file, ddr_cs_n, "ddr_cs_n"); sc_trace(trace_file, ddr_ras_n, "ddr_ras_n");
    sc_trace(trace_file, ddr_cas_n, "ddr_cas_n"); sc_trace(trace_file, ddr_we_n, "ddr_we_n");
    sc_trace(trace_file, ddr_ba, "ddr_ba"); sc_trace(trace_file, ddr_addr, "ddr_addr");
    sc_trace(trace_file, dq_out, "dq_out"); sc_trace(trace_file, dq_in, "dq_in");
    sc_trace(trace_file, dm_out, "dm_out"); sc_trace(trace_file, dqs_out, "dqs_out");
    sc_trace(trace_file, dq_oe, "dq_oe"); sc_trace(trace_file, dqs_oe, "dqs_oe");
#endif

    // Active-high reset used by the DDR2 SystemC design.
    rst.write(1);
    sc_start(15, SC_NS);
    rst.write(0);
    sc_start();

#ifdef WAVE_DUMP
    sc_close_vcd_trace_file(trace_file);
#endif

    return test.failed ? 1 : 0;
}
