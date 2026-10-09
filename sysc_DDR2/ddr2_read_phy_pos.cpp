#include "ddr2_read_phy_pos.h"

void ddr2_read_phy_pos::pos_proc(void)
{
    // Integrated timing: controller produces phy_rd_start on the READ-command
    // posedge.  The positive path therefore uses two RL wait states so the
    // mixed-edge capture order is neg B0, pos B1, neg B2, pos B3.
    enum { ST_IDLE = 0, ST_RL1, ST_RL2, ST_SECOND, ST_HOLD };
    sc_uint<3> state;

    state = ST_IDLE;
    beat1.write(0);
    beat3.write(0);
    wait();

    while (1) {
        switch (state) {
        case ST_IDLE:
            if (phy_rd_start.read()) {
                beat1.write(0);
                beat3.write(0);
                state = ST_RL1;
            }
            break;
        case ST_RL1:
            state = ST_RL2;
            break;
        case ST_RL2:
            beat1.write(dq_in.read());
            state = ST_SECOND;
            break;
        case ST_SECOND:
            beat3.write(dq_in.read());
            state = ST_HOLD;
            break;
        case ST_HOLD:
            if (!phy_rd_start.read()) state = ST_IDLE;
            break;
        default:
            state = ST_IDLE;
            break;
        }
        wait();
    }
}
