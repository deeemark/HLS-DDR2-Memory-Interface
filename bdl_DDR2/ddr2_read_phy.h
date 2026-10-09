#ifndef DDR2_READ_PHY_H
#define DDR2_READ_PHY_H

#include <stdint.h>

typedef struct {
    unsigned state;
    uint16_t rise_reg;
    uint16_t fall_reg;
    uint16_t beat0;
    uint16_t beat1;
    uint16_t beat2;
    uint16_t beat3;
    uint64_t phy_rd_data;
    int phy_rd_valid;
} ddr2_read_phy_t;

void ddr2_read_phy_init(ddr2_read_phy_t *p);
void ddr2_read_phy_edge(ddr2_read_phy_t *p,
                        int clk,
                        int phy_rd_start,
                        uint16_t dq_in);

#endif
