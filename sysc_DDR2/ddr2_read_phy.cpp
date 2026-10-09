#include "ddr2_read_phy.h"

void ddr2_read_phy::control_proc(void)
{
    enum { ST_IDLE = 0, ST_WAIT1, ST_WAIT2, ST_WAIT3, ST_WAIT4, ST_DONE };
    sc_uint<3> state;

    state = ST_IDLE;
    phy_rd_data.write(0);
    phy_rd_valid.write(false);
    wait();

    while (1) {
        phy_rd_valid.write(false);
        switch (state) {
        case ST_IDLE:
            if (phy_rd_start.read()) state = ST_WAIT1;
            break;
        case ST_WAIT1: state = ST_WAIT2; break;
        case ST_WAIT2: state = ST_WAIT3; break;
        case ST_WAIT3: state = ST_WAIT4; break;
        case ST_WAIT4: state = ST_DONE; break;
        case ST_DONE:
            phy_rd_data.write(
                ((sc_uint<64>)beat3_sig.read() << 48) |
                ((sc_uint<64>)beat2_sig.read() << 32) |
                ((sc_uint<64>)beat1_sig.read() << 16) |
                (sc_uint<64>)beat0_sig.read());
            phy_rd_valid.write(true);
            state = ST_IDLE;
            break;
        default:
            state = ST_IDLE;
            break;
        }
        wait();
    }
}
