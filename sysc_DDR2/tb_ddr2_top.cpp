#include "tb_ddr2_top.h"
#include <iostream>
#include <iomanip>

void tb_ddr2_top::run()
{
    req_valid.write(0);
    req_write.write(0);
    req_addr.write(0);
    req_wdata.write(0);
    req_wmask.write(0);
    dq_in.write(0);

    // main.cpp owns reset. Wait until reset has been asserted and released.
    wait(1, SC_NS);
    while (rst.read()) wait(1, SC_NS);

    while (!init_done.read()) wait(5, SC_NS);
    std::cout << "INIT PASS @ " << sc_time_stamp() << std::endl;

    // ---------------- Integrated WRITE ----------------
    while (!req_ready.read()) wait(500, SC_PS);
    req_addr.write(0x000000);
    req_write.write(1);
    req_wdata.write(0x4444333322221111ULL);
    req_wmask.write(0xE4);
    req_valid.write(1);
    wait(10, SC_NS);                 // hold across two CK
    req_valid.write(0);

    while (!(ddr_cs_n.read() == 0 && ddr_ras_n.read() == 1 &&
             ddr_cas_n.read() == 0 && ddr_we_n.read() == 0))
        wait(100, SC_PS);
    std::cout << "WRITE CMD @ " << sc_time_stamp() << std::endl;

    const unsigned exp_dq[4] = {0x1111, 0x2222, 0x3333, 0x4444};
    const unsigned exp_dm[4] = {0, 1, 2, 3};
    unsigned got_dq[4] = {0, 0, 0, 0};
    unsigned got_dm[4] = {0, 0, 0, 0};
    int beats = 0;
    unsigned last_dq = 0xFFFF;
    int guard = 0;

    while (beats < 4 && guard < 1000) {
        wait(100, SC_PS);
        if (dq_oe.read()) {
            unsigned q = dq_out.read().to_uint();
            if (q != last_dq) {
                got_dq[beats] = q;
                got_dm[beats] = dm_out.read().to_uint();
                last_dq = q;
                ++beats;
            }
        }
        ++guard;
    }

    bool write_ok = (beats == 4);
    for (int i = 0; i < beats && i < 4; ++i) {
        std::cout << "WRITE beat" << i << " dq=0x" << std::hex << got_dq[i]
                  << " dm=0x" << got_dm[i] << std::dec << std::endl;
        if (got_dq[i] != exp_dq[i] || got_dm[i] != exp_dm[i]) write_ok = false;
    }

    if (!write_ok) {
        std::cout << "INTEGRATED WRITE FAIL" << std::endl;
        failed = true;
        sc_stop();
        return;
    }
    std::cout << "INTEGRATED WRITE PASS" << std::endl;

    while (!req_ready.read()) wait(500, SC_PS);

    // ---------------- Integrated READ ----------------
    req_addr.write(0x000000);
    req_write.write(0);
    req_wdata.write(0);
    req_wmask.write(0);
    req_valid.write(1);
    wait(10, SC_NS);
    req_valid.write(0);

    while (!(ddr_cs_n.read() == 0 && ddr_ras_n.read() == 1 &&
             ddr_cas_n.read() == 0 && ddr_we_n.read() == 1))
        wait(500, SC_PS);
    std::cout << "READ CMD @ " << sc_time_stamp() << std::endl;

    // Four x16 beats, with setup margin before the mixed-edge captures.
    wait(11500, SC_PS); dq_in.write(0x1111);
    wait(2500, SC_PS);  dq_in.write(0x2222);
    wait(2500, SC_PS);  dq_in.write(0x3333);
    wait(2500, SC_PS);  dq_in.write(0x4444);

    guard = 0;
    while (!rsp_valid.read() && guard < 160) {
        wait(500, SC_PS);
        ++guard;
    }

    std::cout << "RSP valid=" << rsp_valid.read()
              << " data=0x" << std::hex << rsp_rdata.read().to_uint64()
              << std::dec << " @ " << sc_time_stamp() << std::endl;

    if (!rsp_valid.read() ||
        rsp_rdata.read().to_uint64() != 0x4444333322221111ULL) {
        std::cout << "INTEGRATED READ FAIL" << std::endl;
        failed = true;
        sc_stop();
        return;
    }

    std::cout << "INTEGRATED READ PASS" << std::endl;
    std::cout << "DDR2 SYSTEMC INTEGRATED PASS" << std::endl;
    sc_stop();
}
