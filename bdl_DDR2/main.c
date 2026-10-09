// gcc -std=c99 -Wall -Wextra -pedantic -o ddr2_test main.c tb_ddr2_top.c ddr2_top.c ddr2_controller.c ddr2_write_phy.c ddr2_read_phy.c
#include <stdint.h>
#include <string.h>

#include "ddr2_top.h"
#include "tb_ddr2_top.h"

int main(void)
{

    uint8_t  rst       = 0;
    uint8_t  req_valid = 0;
    uint8_t  req_write = 0;
    uint32_t req_addr  = 0;
    uint64_t req_wdata = 0;
    uint8_t  req_wmask = 0;
    uint16_t dq_in     = 0;

    ddr2_top_outputs_t out;
    memset(&out, 0, sizeof(out));

    return tb_ddr2_top_run(&rst,
                           &req_valid,
                           &req_write,
                           &req_addr,
                           &req_wdata,
                           &req_wmask,
                           &dq_in,
                           &out);
}
