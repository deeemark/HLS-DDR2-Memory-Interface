#ifndef DDR2_CONTROLLER_H
#define DDR2_CONTROLLER_H

#include <stdint.h>

/* C behavioral interface.  Not used by the BDL build. */
extern uint8_t  rst;
extern uint8_t  req_valid;
extern uint8_t  req_write;
extern uint32_t req_addr;
extern uint64_t req_wdata;
extern uint8_t  req_wmask;
extern uint8_t  req_ready;
extern uint8_t  rsp_valid;
extern uint64_t rsp_rdata;
extern uint8_t  ddr_cke, ddr_cs_n, ddr_ras_n, ddr_cas_n, ddr_we_n, ddr_odt;
extern uint8_t  ddr_ba;
extern uint16_t ddr_addr;
extern uint8_t  init_done;
extern uint8_t  phy_wr_start;
extern uint64_t phy_wr_data;
extern uint8_t  phy_wr_mask;
extern uint8_t  phy_rd_start;
extern uint8_t  phy_rd_valid;
extern uint64_t phy_rd_data;

/* Exposed for behavioral checking/debug. */
extern uint8_t  state;
extern uint16_t wait_count, dll_count, refresh_count;

void ddr2_controller_init(void);
void ddr2_controller_step(void);

#endif
