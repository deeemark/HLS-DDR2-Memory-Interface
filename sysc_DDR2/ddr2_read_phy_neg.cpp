#include "ddr2_read_phy_neg.h"

void ddr2_read_phy_neg::neg_proc(void)
{
    enum { ST_IDLE = 0, ST_RL1, ST_RL2, ST_SECOND, ST_HOLD };
    sc_uint<3> state;

    state = ST_IDLE;
    beat0.write(0);
    beat2.write(0);
    wait();

    while (1) {
        switch (state) {
        case ST_IDLE:
            if (phy_rd_start.read()) {
                beat0.write(0);
                beat2.write(0);
                state = ST_RL1;
            }
            break;
        case ST_RL1:
            state = ST_RL2;
            break;
        case ST_RL2:
            beat0.write(dq_in.read());
            state = ST_SECOND;
            break;
        case ST_SECOND:
            beat2.write(dq_in.read());
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
