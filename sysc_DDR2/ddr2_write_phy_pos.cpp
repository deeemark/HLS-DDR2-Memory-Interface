#include "ddr2_write_phy_pos.h"

void ddr2_write_phy_pos::pos_proc(void)
{
    const unsigned ST_IDLE      = 0;
    const unsigned ST_WL1       = 1;
    const unsigned ST_BURST0    = 2;
    const unsigned ST_BURST1    = 3;
    const unsigned ST_POSTAMBLE = 4;

    sc_uint<3> phase = ST_IDLE;
    sc_uint<64> wr_data_reg = 0;
    sc_uint<8> wr_mask_reg = 0;

    dq_rise.write(0);
    dm_rise.write(0);
    dqs_rise.write(0);
    dq_fall_next.write(0);
    dm_fall_next.write(0);
    dqs_fall_next.write(0);
    dq_oe.write(false);
    dqs_oe.write(false);
    phy_wr_busy.write(false);
    phy_wr_done.write(false);

    wait();

    while (1)
    {
        dq_rise.write(0);
        dm_rise.write(0);
        dqs_rise.write(0);
        dq_fall_next.write(0);
        dm_fall_next.write(0);
        dqs_fall_next.write(0);
        dq_oe.write(false);
        dqs_oe.write(false);
        phy_wr_busy.write(false);
        phy_wr_done.write(false);

        switch (phase)
        {
            case ST_IDLE:
                if (phy_wr_start.read()) {
                    wr_data_reg = phy_wr_data.read();
                    wr_mask_reg = phy_wr_mask.read();
                    phase = ST_WL1;
                    dqs_oe.write(true);
                    phy_wr_busy.write(true);
                }
                break;

            case ST_WL1:
                dq_rise.write(wr_data_reg.range(15, 0));
                dq_fall_next.write(wr_data_reg.range(31, 16));
                dm_rise.write(wr_mask_reg.range(1, 0));
                dm_fall_next.write(wr_mask_reg.range(3, 2));
                dqs_rise.write(3);
                dqs_fall_next.write(0);
                dq_oe.write(true);
                dqs_oe.write(true);
                phy_wr_busy.write(true);
                phase = ST_BURST0;
                break;

            case ST_BURST0:
                dq_rise.write(wr_data_reg.range(47, 32));
                dq_fall_next.write(wr_data_reg.range(63, 48));
                dm_rise.write(wr_mask_reg.range(5, 4));
                dm_fall_next.write(wr_mask_reg.range(7, 6));
                dqs_rise.write(3);
                dqs_fall_next.write(0);
                dq_oe.write(true);
                dqs_oe.write(true);
                phy_wr_busy.write(true);
                phase = ST_BURST1;
                break;

            case ST_BURST1:
                dq_oe.write(false);
                dqs_oe.write(true);
                phy_wr_busy.write(true);
                phase = ST_POSTAMBLE;
                break;

            case ST_POSTAMBLE:
                dq_oe.write(false);
                dqs_oe.write(true);
                phy_wr_busy.write(true);
                phy_wr_done.write(true);
                phase = ST_IDLE;
                break;

            default:
                phase = ST_IDLE;
                wr_data_reg = 0;
                wr_mask_reg = 0;
                break;
        }

        wait();
    }
}
