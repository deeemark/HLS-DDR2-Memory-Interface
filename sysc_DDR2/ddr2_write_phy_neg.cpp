#include "ddr2_write_phy_neg.h"

void ddr2_write_phy_neg::neg_proc(void)
{
    dq_fall.write(0);
    dm_fall.write(0);
    dqs_fall.write(0);

    wait();

    while (1)
    {
        dq_fall.write(dq_in.read());
        dm_fall.write(dm_in.read());
        dqs_fall.write(dqs_in.read());
        wait();
    }
}
