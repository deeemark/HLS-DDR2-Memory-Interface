#ifndef TB_DDR2_TOP_H
#define TB_DDR2_TOP_H

#include <stdint.h>
#include "ddr2_top.h"

int tb_ddr2_top_run(
    uint8_t *rst,
    uint8_t *req_valid,
    uint8_t *req_write,
    uint32_t *req_addr,
    uint64_t *req_wdata,
    uint8_t *req_wmask,
    uint16_t *dq_in,
    ddr2_top_outputs_t *out);

#endif
