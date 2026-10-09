//========================================================================================
// File Name    : ddr2_write_phy.c
// Description  : The High level implementation of the DDR2-400B write PHY
// Handles the actual write across DDR edges
// Timing information:
// CK = 200 MHz (5 ns), WL = 2 CK, BL4 = four x16 transfers = two CK
//========================================================================================


 // Interface from controller:
 //  phy_wr_start : one-cycle pulse when WRITE command is issued
 //  phy_wr_data  : 64-bit BL4 payload. The PHY serializes it into 4 different beats
 //  phy_wr_mask  : 8 byte-mask bits, two mask bits per x16 beat

 // The Write PHY states
#define ST_IDLE       0
#define ST_WL1        1
#define ST_BURST0     2
#define ST_BURST1     3
#define ST_POSTAMBLE  4

#ifndef C_SIM

clock CLK;
reset rst;

in  ter(0:1)  phy_wr_start;
in  ter(0:64) phy_wr_data;
in  ter(0:8)  phy_wr_mask;

out ter(0:16) dq_out;
out ter(0:2)  dm_out;
out ter(0:2)  dqs_out;
out ter(0:1)  dq_oe;
out ter(0:1)  dqs_oe;
out ter(0:1)  phy_wr_busy;
out ter(0:1)  phy_wr_done;

var(0:3) phase;

// PHY specific copy of data/mask
var(0:64) wr_data_reg;
var(0:8)  wr_mask_reg;

// storage to be associated with opposite clock edges for edge-specific logic
var(0:16) dq_rise_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = pos */;

var(0:16) dq_fall_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = neg */;

var(0:2) dm_rise_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = pos */;

var(0:2) dm_fall_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = neg */;

var(0:2) dqs_rise_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = pos */;

var(0:2) dqs_fall_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = neg */;

process ddr2_write_phy()
{
    phase = ST_IDLE;
    wr_data_reg = 0;
    wr_mask_reg = 0;

    dq_rise_reg = 0;
    dq_fall_reg = 0;
    dm_rise_reg = 0;
    dm_fall_reg = 0;
    dqs_rise_reg = 0;
    dqs_fall_reg = 0;

    dq_out = 0;
    dm_out = 0;
    dqs_out = 0;
    dq_oe = 0;
    dqs_oe = 0;
    phy_wr_busy = 0;
    phy_wr_done = 0;

    while (1)
    {
        if (CLK == 1)
        {
            dq_out = dq_rise_reg;
            dm_out = dm_rise_reg;
            dqs_out = dqs_rise_reg;
        }
        else
        {
            dq_out = dq_fall_reg;
            dm_out = dm_fall_reg;
            dqs_out = dqs_fall_reg;
        }

        if ((phase == ST_WL1) ||
            (phase == ST_BURST0) ||
            (phase == ST_BURST1) ||
            (phase == ST_POSTAMBLE))
            dqs_oe = 1;
        else
            dqs_oe = 0;

        if ((phase == ST_BURST0) || (phase == ST_BURST1))
            dq_oe = 1;
        else
            dq_oe = 0;

        if (phase == ST_IDLE)
            phy_wr_busy = 0;
        else
            phy_wr_busy = 1;

        if (phase == ST_POSTAMBLE)
            phy_wr_done = 1;
        else
            phy_wr_done = 0;

        $

        // Load the edge registers that will be observed on subsequent high/low clock phases.
         // BL4 beat mapping for phy_wr_data:
         //   beat 0 = [15:0]
         //   beat 1 = [31:16]
         //   beat 2 = [47:32]
         //   beat 3 = [63:48]
         // Sequence: IDLE -> WL1 -> BURSTO -> BURST1 -> POSTAMBLE -> IDLE 
        if (rst == 1)
        {
            phase = ST_IDLE;
            wr_data_reg = 0;
            wr_mask_reg = 0;

            dq_rise_reg = 0;
            dq_fall_reg = 0;
            dm_rise_reg = 0;
            dm_fall_reg = 0;
            dqs_rise_reg = 0;
            dqs_fall_reg = 0;
        }
        else if (phase == ST_IDLE)
        {
            if (phy_wr_start == 1)
            {
                phase = ST_WL1;
                wr_data_reg = phy_wr_data;
                wr_mask_reg = phy_wr_mask;

                dq_rise_reg = 0;
                dq_fall_reg = 0;
                dm_rise_reg = 0;
                dm_fall_reg = 0;
                dqs_rise_reg = 0;
                dqs_fall_reg = 0;
            }
            else
            {
                phase = ST_IDLE;
                wr_data_reg = wr_data_reg;
                wr_mask_reg = wr_mask_reg;

                dq_rise_reg = 0;
                dq_fall_reg = 0;
                dm_rise_reg = 0;
                dm_fall_reg = 0;
                dqs_rise_reg = 0;
                dqs_fall_reg = 0;
            }
        }
        else if (phase == ST_WL1)
        {
            // First CK of BL4:
            //   rising transfer  = beat 0
            //   falling transfer = beat 1
            phase = ST_BURST0;
            wr_data_reg = wr_data_reg;
            wr_mask_reg = wr_mask_reg;

            dq_rise_reg = wr_data_reg(48:16);
            dq_fall_reg = wr_data_reg(32:16);

            dm_rise_reg = wr_mask_reg(6:2);
            dm_fall_reg = wr_mask_reg(4:2);

            dqs_rise_reg = 3;
            dqs_fall_reg = 0;
        }
        else if (phase == ST_BURST0)
        {
            // Second CK of BL4:
            //   rising transfer  = beat 2
            //   falling transfer = beat 3
            phase = ST_BURST1;
            wr_data_reg = wr_data_reg;
            wr_mask_reg = wr_mask_reg;

            dq_rise_reg = wr_data_reg(16:16);
            dq_fall_reg = wr_data_reg(0:16);

            dm_rise_reg = wr_mask_reg(2:2);
            dm_fall_reg = wr_mask_reg(0:2);

            dqs_rise_reg = 3;
            dqs_fall_reg = 0;
        }
        else if (phase == ST_BURST1)
        {
            phase = ST_POSTAMBLE;
            wr_data_reg = wr_data_reg;
            wr_mask_reg = wr_mask_reg;

            dq_rise_reg = 0;
            dq_fall_reg = 0;
            dm_rise_reg = 0;
            dm_fall_reg = 0;
            dqs_rise_reg = 0;
            dqs_fall_reg = 0;
        }
        else if (phase == ST_POSTAMBLE)
        {
            phase = ST_IDLE;
            wr_data_reg = wr_data_reg;
            wr_mask_reg = wr_mask_reg;

            dq_rise_reg = 0;
            dq_fall_reg = 0;
            dm_rise_reg = 0;
            dm_fall_reg = 0;
            dqs_rise_reg = 0;
            dqs_fall_reg = 0;
        }
        else
        {
            phase = ST_IDLE;
            wr_data_reg = 0;
            wr_mask_reg = 0;

            dq_rise_reg = 0;
            dq_fall_reg = 0;
            dm_rise_reg = 0;
            dm_fall_reg = 0;
            dqs_rise_reg = 0;
            dqs_fall_reg = 0;
        }

        $
    }
}

#else

#include <stdint.h>
#include <string.h>
#include "ddr2_write_phy.h"

static void clear_edges(ddr2_write_phy_t *p)
{
    p->dq_rise_reg = 0;
    p->dq_fall_reg = 0;
    p->dm_rise_reg = 0;
    p->dm_fall_reg = 0;
    p->dqs_rise_reg = 0;
    p->dqs_fall_reg = 0;
}

void ddr2_write_phy_init(ddr2_write_phy_t *p)
{
    memset(p, 0, sizeof(*p));
    p->phase = ST_IDLE;
}

static void drive_outputs(ddr2_write_phy_t *p, int clk)
{
    if (clk) {
        p->dq_out  = p->dq_rise_reg;
        p->dm_out  = p->dm_rise_reg;
        p->dqs_out = p->dqs_rise_reg;
    } else {
        p->dq_out  = p->dq_fall_reg;
        p->dm_out  = p->dm_fall_reg;
        p->dqs_out = p->dqs_fall_reg;
    }

    p->dqs_oe = (p->phase == ST_WL1 ||
                 p->phase == ST_BURST0 ||
                 p->phase == ST_BURST1 ||
                 p->phase == ST_POSTAMBLE);

    p->dq_oe = (p->phase == ST_BURST0 ||
                p->phase == ST_BURST1);

    p->phy_wr_busy = (p->phase != ST_IDLE);
    p->phy_wr_done = (p->phase == ST_POSTAMBLE);
}

/*
 * Advance the state machine once per rising CK edge.
 * The normal-C testbench calls ddr2_write_phy_eval() on both clock levels
 * so the stored rising/falling values are visible at half-cycle resolution.
 */
void ddr2_write_phy_tick(ddr2_write_phy_t *p,
                         int rst,
                         int phy_wr_start,
                         uint64_t phy_wr_data,
                         uint8_t phy_wr_mask)
{
    if (rst) {
        p->phase = ST_IDLE;
        p->wr_data_reg = 0;
        p->wr_mask_reg = 0;
        clear_edges(p);
        return;
    }

    if (p->phase == ST_IDLE) {
        if (phy_wr_start) {
            p->phase = ST_WL1;
            p->wr_data_reg = phy_wr_data;
            p->wr_mask_reg = phy_wr_mask;
            clear_edges(p);
        } else {
            clear_edges(p);
        }
    }
    else if (p->phase == ST_WL1) {
        p->phase = ST_BURST0;

        /* Same CWB bit slices:
         *   wr_data_reg(48:16) -> bits [15:0]
         *   wr_data_reg(32:16) -> bits [31:16]
         */
        p->dq_rise_reg = (uint16_t)(p->wr_data_reg >> 0);
        p->dq_fall_reg = (uint16_t)(p->wr_data_reg >> 16);

        p->dm_rise_reg = (uint8_t)((p->wr_mask_reg >> 0) & 0x3u);
        p->dm_fall_reg = (uint8_t)((p->wr_mask_reg >> 2) & 0x3u);

        p->dqs_rise_reg = 3;
        p->dqs_fall_reg = 0;
    }
    else if (p->phase == ST_BURST0) {
        p->phase = ST_BURST1;

        p->dq_rise_reg = (uint16_t)(p->wr_data_reg >> 32);
        p->dq_fall_reg = (uint16_t)(p->wr_data_reg >> 48);

        p->dm_rise_reg = (uint8_t)((p->wr_mask_reg >> 4) & 0x3u);
        p->dm_fall_reg = (uint8_t)((p->wr_mask_reg >> 6) & 0x3u);

        p->dqs_rise_reg = 3;
        p->dqs_fall_reg = 0;
    }
    else if (p->phase == ST_BURST1) {
        p->phase = ST_POSTAMBLE;
        clear_edges(p);
    }
    else if (p->phase == ST_POSTAMBLE) {
        p->phase = ST_IDLE;
        clear_edges(p);
    }
    else {
        p->phase = ST_IDLE;
        p->wr_data_reg = 0;
        p->wr_mask_reg = 0;
        clear_edges(p);
    }
}

void ddr2_write_phy_eval(ddr2_write_phy_t *p, int clk)
{
    drive_outputs(p, clk);
}

#endif
