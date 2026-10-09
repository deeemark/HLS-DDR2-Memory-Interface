#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "ddr2_top.h"
#include "tb_ddr2_top.h"

static int failures = 0;

static void full_cycle(
    uint8_t rst,
    uint8_t req_valid,
    uint8_t req_write,
    uint32_t req_addr,
    uint64_t req_wdata,
    uint8_t req_wmask,
    uint16_t dq_fall,
    uint16_t dq_rise,
    ddr2_top_outputs_t *out)
{
    ddr2_top_falling(rst, dq_fall, out);
    ddr2_top_rising(rst, req_valid, req_write, req_addr,
                    req_wdata, req_wmask, dq_rise, out);
}

static int wait_ready(
    int limit,
    uint8_t rst,
    uint8_t req_valid,
    uint8_t req_write,
    uint32_t req_addr,
    uint64_t req_wdata,
    uint8_t req_wmask,
    ddr2_top_outputs_t *out)
{
    int i;
    for (i = 0; i < limit; ++i) {
        full_cycle(rst, req_valid, req_write, req_addr,
                   req_wdata, req_wmask, 0, 0, out);
        if (out->init_done && out->req_ready)
            return i + 1;
    }
    return -1;
}

int tb_ddr2_top_run(
    uint8_t *rst,
    uint8_t *req_valid,
    uint8_t *req_write,
    uint32_t *req_addr,
    uint64_t *req_wdata,
    uint8_t *req_wmask,
    uint16_t *dq_in,
    ddr2_top_outputs_t *out)
{
    int i, n;
    int saw_write_start = 0;
    int saw_read_start = 0;
    int saw_rsp = 0;
    int capture_phase = -1;
    uint16_t wr_beats[4] = {0,0,0,0};
    uint8_t  wr_dm[4] = {0,0,0,0};
    int wr_count = 0;
    uint32_t test_addr = 0;
    uint64_t payload = 0;
    uint8_t test_mask = 0;
    uint16_t read_beats[4] = {0,0,0,0};
    uint16_t golden_wr_beats[4] = {0,0,0,0};
    uint8_t golden_wr_dm[4] = {0,0,0,0};
    uint64_t golden_read = 0;
    FILE *fp;

    failures = 0;

    fp = fopen("ddr2_input.txt", "r");
    if (!fp) {
        printf("FAIL: could not open ddr2_input.txt\n");
        return 1;
    }
    if (fscanf(fp, "%x %" SCNx64 " %hhx %hx %hx %hx %hx",
               &test_addr, &payload, &test_mask,
               &read_beats[0], &read_beats[1],
               &read_beats[2], &read_beats[3]) != 7) {
        printf("FAIL: invalid ddr2_input.txt format\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    fp = fopen("ddr2_output_golden.txt", "r");
    if (!fp) {
        printf("FAIL: could not open ddr2_output_golden.txt\n");
        return 1;
    }
    if (fscanf(fp, "%hx %hhx %hx %hhx %hx %hhx %hx %hhx %" SCNx64,
               &golden_wr_beats[0], &golden_wr_dm[0],
               &golden_wr_beats[1], &golden_wr_dm[1],
               &golden_wr_beats[2], &golden_wr_dm[2],
               &golden_wr_beats[3], &golden_wr_dm[3],
               &golden_read) != 9) {
        printf("FAIL: invalid ddr2_output_golden.txt format\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);

    printf("DDR2 top-level behavioral integration test\n");

    ddr2_top_init();

    *rst = 1;
    full_cycle(*rst, *req_valid, *req_write, *req_addr,
               *req_wdata, *req_wmask, 0, 0, out);
    *rst = 0;

    n = wait_ready(50000, *rst, *req_valid, *req_write, *req_addr,
                   *req_wdata, *req_wmask, out);
    if (n < 0) {
        printf("FAIL: initialization did not reach READY\n");
        return 1;
    }
    printf("PASS: initialization reached READY\n");

    *req_write = 1;
    *req_addr  = test_addr;
    *req_wdata = payload;
    *req_wmask = test_mask;
    *req_valid = 1;

    for (i = 0; i < 40; ++i) {
        ddr2_top_falling(*rst, 0, out);
        if (out->dq_oe && wr_count < 4) {
            wr_beats[wr_count] = out->dq_out;
            wr_dm[wr_count] = out->dm_out;
            wr_count++;
        }

        ddr2_top_rising(*rst, *req_valid, *req_write, *req_addr,
                        *req_wdata, *req_wmask, 0, out);

        if (out->phy_wr_start) {
            saw_write_start = 1;
            *req_valid = 0;
        }

        if (out->dq_oe && wr_count < 4) {
            wr_beats[wr_count] = out->dq_out;
            wr_dm[wr_count] = out->dm_out;
            wr_count++;
        }

        if (saw_write_start && out->req_ready)
            break;
    }

    if (!saw_write_start) {
        printf("FAIL: controller never started write PHY\n");
        failures++;
    }

    if (wr_count < 4 ||
        wr_beats[0] != golden_wr_beats[0] ||
        wr_beats[1] != golden_wr_beats[1] ||
        wr_beats[2] != golden_wr_beats[2] ||
        wr_beats[3] != golden_wr_beats[3]) {
        printf("FAIL: write PHY beats:");
        for (i = 0; i < wr_count && i < 4; ++i)
            printf(" %04X", wr_beats[i]);
        printf("\n");
        failures++;
    } else {
        printf("PASS: WRITE serialized %04X -> %04X -> %04X -> %04X\n",
               wr_beats[0], wr_beats[1], wr_beats[2], wr_beats[3]);
    }

    if (wr_count >= 4 &&
        (wr_dm[0] != golden_wr_dm[0] || wr_dm[1] != golden_wr_dm[1] ||
         wr_dm[2] != golden_wr_dm[2] || wr_dm[3] != golden_wr_dm[3])) {
        printf("FAIL: write DM sequence %u -> %u -> %u -> %u\n",
               wr_dm[0], wr_dm[1], wr_dm[2], wr_dm[3]);
        failures++;
    } else if (wr_count >= 4) {
        printf("PASS: WRITE DM sequence %u -> %u -> %u -> %u\n",
               wr_dm[0], wr_dm[1], wr_dm[2], wr_dm[3]);
    }

    *req_valid = 0;
    if (!out->req_ready &&
        wait_ready(40, *rst, *req_valid, *req_write, *req_addr,
                   *req_wdata, *req_wmask, out) < 0) {
        printf("FAIL: controller did not return READY after WRITE\n");
        return 1;
    }

    *req_write = 0;
    *req_addr = test_addr;
    *req_wdata = 0;
    *req_wmask = 0;
    *req_valid = 1;

    for (i = 0; i < 60; ++i) {
        uint16_t dq_fall = 0;
        uint16_t dq_rise = 0;

        if (capture_phase == 0) {
            dq_fall = read_beats[0];
            dq_rise = read_beats[1];
        } else if (capture_phase == 1) {
            dq_fall = read_beats[2];
            dq_rise = read_beats[3];
        }

        *dq_in = dq_fall;
        ddr2_top_falling(*rst, *dq_in, out);

        *dq_in = dq_rise;
        ddr2_top_rising(*rst, *req_valid, *req_write, *req_addr,
                        *req_wdata, *req_wmask, *dq_in, out);

        if (out->phy_rd_start) {
            saw_read_start = 1;
            *req_valid = 0;
            capture_phase = -3;
        }

        if (saw_read_start && capture_phase < 2)
            capture_phase++;

        if (out->rsp_valid) {
            saw_rsp = 1;
            if (out->rsp_rdata != golden_read) {
                printf("FAIL: READ response=%016" PRIX64
                       " expected=%016" PRIX64 "\n",
                       out->rsp_rdata, golden_read);
                failures++;
            } else {
                printf("PASS: READ response=%016" PRIX64 "\n", out->rsp_rdata);
            }
            break;
        }
    }

    if (!saw_read_start) {
        printf("FAIL: controller never started read PHY\n");
        failures++;
    }
    if (!saw_rsp) {
        printf("FAIL: controller never produced READ response\n");
        failures++;
    }

    if (failures) {
        printf("DDR2 TOP BEHAVIORAL TEST: FAIL (%d checks)\n", failures);
        return 1;
    }

    printf("PASS: controller + write PHY + read PHY integrated\n");
    printf("DDR2 TOP BEHAVIORAL TEST: SUCCESS\n");
    return 0;
}
