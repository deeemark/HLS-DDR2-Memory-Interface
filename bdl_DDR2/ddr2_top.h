#ifndef DDR2_TOP_H
#define DDR2_TOP_H

#include <stdint.h>

typedef struct {
    uint8_t  req_ready;
    uint8_t  rsp_valid;
    uint64_t rsp_rdata;
    uint8_t  init_done;

    uint8_t  phy_wr_start;
    uint8_t  phy_rd_start;

    uint16_t dq_out;
    uint8_t  dm_out;
    uint8_t  dqs_out;
    uint8_t  dq_oe;
    uint8_t  dqs_oe;
    uint8_t  phy_wr_busy;
    uint8_t  phy_wr_done;
} ddr2_top_outputs_t;

void ddr2_top_init(void);

void ddr2_top_rising(
    uint8_t rst,
    uint8_t req_valid,
    uint8_t req_write,
    uint32_t req_addr,
    uint64_t req_wdata,
    uint8_t req_wmask,
    uint16_t dq_in,
    ddr2_top_outputs_t *out);

void ddr2_top_falling(
    uint8_t rst,
    uint16_t dq_in,
    ddr2_top_outputs_t *out);

#endif
