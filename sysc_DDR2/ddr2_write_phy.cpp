#include "ddr2_write_phy.h"

void ddr2_write_phy::output_mux(void)
{
    if (clk.read()) {
        dq_out.write(dq_rise_sig.read());
        dm_out.write(dm_rise_sig.read());
        dqs_out.write(dqs_rise_sig.read());
    }
    else {
        dq_out.write(dq_fall_sig.read());
        dm_out.write(dm_fall_sig.read());
        dqs_out.write(dqs_fall_sig.read());
    }
}
