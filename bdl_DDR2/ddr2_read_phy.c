//========================================================================================
// File Name    : ddr2_read_phy.c
// Description  : The High level implementation of the  DDR2 x16 BL4 logical mixed-edge read PHY
// Synthesize in manual scheduling
// Handles the actual read across DDR edges: takes in four beats, saves them,
// and reconstructs them into 64 bits
// Timing information:
// CK = 200 MHz (5 ns), CL = 3, AL = 0, RL = AL + CL = 3 CK, BL4 = four x16 transfers
//========================================================================================

 // Capture order:
 //   beat0 = 1111
 //   beat1 = 2222
 //   beat2 = 3333
 //   beat3 = 4444
 //   phy_rd_data = 4444333322221111

#ifndef C_SIM

clock CLK;
reset rst;

in  ter(0:1)  phy_rd_start;
in  ter(0:16) dq_in;

out ter(0:64) phy_rd_data;
out ter(0:1)  phy_rd_valid;

reg(0:16) rise_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = pos */;

reg(0:16) fall_reg
    /* Cyber clock_sig = CLK */
    /* Cyber clock_pol = neg */;

reg(0:16) beat0;
reg(0:16) beat1;
reg(0:16) beat2;
reg(0:16) beat3;

process ddr2_read_phy_manual()
{
    while (1)
    {
        phy_rd_valid = 0;
        phy_rd_data = 0;

        // Wait for READ-command trigger from controller.
        while (phy_rd_start == 0)
        {
            $
        }

        // RL timing delay.
        $
        $

        // Capture window 0. 
        fall_reg = dq_in;
        rise_reg = dq_in;
        $

        beat0 = fall_reg;
        beat1 = rise_reg;
        $

        // Capture window 1.
        fall_reg = dq_in;
        rise_reg = dq_in;
        $

        beat2 = fall_reg;
        beat3 = rise_reg;
        $

        phy_rd_data(0:16)  = beat3;
        phy_rd_data(16:16) = beat2;
        phy_rd_data(32:16) = beat1;
        phy_rd_data(48:16) = beat0;
        phy_rd_valid = 1;
        $

        phy_rd_valid = 0;
        $
    }
}

#else

#include <stdint.h>
#include <string.h>
#include "ddr2_read_phy.h"

enum {
    RD_IDLE = 0,
    RD_RL1,
    RD_RL2,
    RD_CAPTURE0,
    RD_SAVE0,
    RD_CAPTURE1,
    RD_SAVE1,
    RD_VALID,
    RD_CLEAR
};

void ddr2_read_phy_init(ddr2_read_phy_t *p)
{
    memset(p, 0, sizeof(*p));
    p->state = RD_IDLE;
}

/*
 * Model one half-cycle edge.
 *
 * The established BDL implementation captures the chronological stream as:
 *   first capture window:  falling -> beat0, rising -> beat1
 *   second capture window: falling -> beat2, rising -> beat3
 *
 * clk=0 denotes the falling edge and clk=1 denotes the rising edge.
 */
void ddr2_read_phy_edge(ddr2_read_phy_t *p,
                        int clk,
                        int phy_rd_start,
                        uint16_t dq_in)
{
    p->phy_rd_valid = 0;

    switch (p->state) {
    case RD_IDLE:
        p->phy_rd_data = 0;
        if (phy_rd_start && clk == 1)
            p->state = RD_RL1;
        break;

    case RD_RL1:
        if (clk == 1)
            p->state = RD_RL2;
        break;

    case RD_RL2:
        if (clk == 1)
            p->state = RD_CAPTURE0;
        break;

    case RD_CAPTURE0:
        if (clk == 0) {
            p->fall_reg = dq_in;
            p->beat0 = p->fall_reg;
        } else {
            p->rise_reg = dq_in;
            p->beat1 = p->rise_reg;
            p->state = RD_CAPTURE1;
        }
        break;

    case RD_CAPTURE1:
        if (clk == 0) {
            p->fall_reg = dq_in;
            p->beat2 = p->fall_reg;
        } else {
            p->rise_reg = dq_in;
            p->beat3 = p->rise_reg;

            p->phy_rd_data =
                ((uint64_t)p->beat3 << 48) |
                ((uint64_t)p->beat2 << 32) |
                ((uint64_t)p->beat1 << 16) |
                ((uint64_t)p->beat0);

            p->phy_rd_valid = 1;
            p->state = RD_VALID;
        }
        break;

    case RD_VALID:
        /* Hold valid for the remainder of this CK; clear next rising edge. */
        if (clk == 1) {
            p->phy_rd_valid = 0;
            p->state = RD_IDLE;
        } else {
            p->phy_rd_valid = 1;
        }
        break;

    default:
        p->state = RD_IDLE;
        p->phy_rd_valid = 0;
        p->phy_rd_data = 0;
        break;
    }
}

#endif
