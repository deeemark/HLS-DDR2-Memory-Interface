#ifndef DDR2_WRITE_PHY_H
#define DDR2_WRITE_PHY_H

#include <stdint.h>

typedef struct {
    unsigned phase;

    uint64_t wr_data_reg;
    uint8_t  wr_mask_reg;

    uint16_t dq_rise_reg;
    uint16_t dq_fall_reg;
    uint8_t  dm_rise_reg;
    uint8_t  dm_fall_reg;
    uint8_t  dqs_rise_reg;
    uint8_t  dqs_fall_reg;

    uint16_t dq_out;
    uint8_t  dm_out;
    uint8_t  dqs_out;
    int dq_oe;
    int dqs_oe;
    int phy_wr_busy;
    int phy_wr_done;
} ddr2_write_phy_t;

void ddr2_write_phy_init(ddr2_write_phy_t *p);

void ddr2_write_phy_tick(ddr2_write_phy_t *p,
                         int rst,
                         int phy_wr_start,
                         uint64_t phy_wr_data,
                         uint8_t phy_wr_mask);

void ddr2_write_phy_eval(ddr2_write_phy_t *p, int clk);

#endif
