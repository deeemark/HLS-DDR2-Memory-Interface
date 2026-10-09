#ifndef C_SIM

//========================================================================================
// File Name    : ddr2_top.c
// Description  : DDR2 controller + mixed-edge write PHY + mixed-edge read PHY structural integration
// Controller:
//  transaction scheduling
//  DDR commands
//  initialization
//  refresh
//  timing enforcement
// Write PHY:
//  64-bit -> x16 DDR serialization
//  DQ/DM/DQS generation
// Read PHY:
//  x16 DDR capture
//  four beats -> 64-bit reconstruction
//========================================================================================

// Synthesized lower module: ddr2_controller
defmod ddr2_controller {
    clock CLK/* Cyber clock_edge = pos */;
    reset rst/* Cyber reset_mode = async, reset_active = high */;

    in  unsigned ter(0..0)  req_valid;
    in  unsigned ter(0..0)  req_write;
    in  unsigned ter(0..23) req_addr;
    in  unsigned ter(0..63) req_wdata;
    in  unsigned ter(0..7)  req_wmask;
    out unsigned ter(0..0)  req_ready;
    out unsigned ter(0..0)  rsp_valid;
    out unsigned ter(0..63) rsp_rdata;

    out unsigned ter(0..0)  ddr_cke;
    out unsigned ter(0..0)  ddr_cs_n;
    out unsigned ter(0..0)  ddr_ras_n;
    out unsigned ter(0..0)  ddr_cas_n;
    out unsigned ter(0..0)  ddr_we_n;
    out unsigned ter(0..0)  ddr_odt;
    out unsigned ter(0..1)  ddr_ba;
    out unsigned ter(0..12) ddr_addr;
    out unsigned ter(0..0)  init_done;

    out unsigned ter(0..0)  phy_wr_start;
    out unsigned ter(0..63) phy_wr_data;
    out unsigned ter(0..7)  phy_wr_mask;

    out unsigned ter(0..0)  phy_rd_start;
    in  unsigned ter(0..0)  phy_rd_valid;
    in  unsigned ter(0..63) phy_rd_data;
} INST_controller;

// Synthesized lower module: ddr2_write_phy
defmod ddr2_write_phy {
    clock CLK/* Cyber clock_edge = pos */;
    reset rst/* Cyber reset_mode = async, reset_active = high */;

    in  unsigned ter(0..0)  phy_wr_start;
    in  unsigned ter(0..63) phy_wr_data;
    in  unsigned ter(0..7)  phy_wr_mask;

    out unsigned ter(0..15) dq_out;
    out unsigned ter(0..1)  dm_out;
    out unsigned ter(0..1)  dqs_out;
    out unsigned ter(0..0)  dq_oe;
    out unsigned ter(0..0)  dqs_oe;
    out unsigned ter(0..0)  phy_wr_busy;
    out unsigned ter(0..0)  phy_wr_done;
} INST_write_phy;

// Synthesized lower module: ddr2_read_phy
defmod ddr2_read_phy_manual {
    clock CLK/* Cyber clock_edge = pos */;
    reset rst/* Cyber reset_mode = async, reset_active = high */;

    in  unsigned ter(0..0)  phy_rd_start;
    in  unsigned ter(0..15) dq_in;

    out unsigned ter(0..63) phy_rd_data;
    out unsigned ter(0..0)  phy_rd_valid;
} INST_read_phy;

clock CLK/* Cyber clock_edge = pos */;
reset rst/* Cyber reset_mode = async, reset_active = high */;

// Request/response interface 
in  unsigned ter(0..0)  req_valid;
in  unsigned ter(0..0)  req_write;
in  unsigned ter(0..23) req_addr;
in  unsigned ter(0..63) req_wdata;
in  unsigned ter(0..7)  req_wmask;
out unsigned ter(0..0)  req_ready;
out unsigned ter(0..0)  rsp_valid;
out unsigned ter(0..63) rsp_rdata;

// DDR2 command/address pins
out unsigned ter(0..0)  ddr_cke;
out unsigned ter(0..0)  ddr_cs_n;
out unsigned ter(0..0)  ddr_ras_n;
out unsigned ter(0..0)  ddr_cas_n;
out unsigned ter(0..0)  ddr_we_n;
out unsigned ter(0..0)  ddr_odt;
out unsigned ter(0..1)  ddr_ba;
out unsigned ter(0..12) ddr_addr;
out unsigned ter(0..0)  init_done;

// Mixed-edge DDR2 write PHY outputs
out unsigned ter(0..15) dq_out;
out unsigned ter(0..1)  dm_out;
out unsigned ter(0..1)  dqs_out;
out unsigned ter(0..0)  dq_oe;
out unsigned ter(0..0)  dqs_oe;

// DDR2 read data input (logical x16 DQ input path) 
in unsigned ter(0..15) dq_in;

// Exposed write-PHY integration/debug 
out unsigned ter(0..0) phy_wr_busy;
out unsigned ter(0..0) phy_wr_done;

process DDR2_TOP()
{
    unsigned ter(0..0)  phy_wr_start_i;
    unsigned ter(0..63) phy_wr_data_i;
    unsigned ter(0..7)  phy_wr_mask_i;

    unsigned ter(0..0)  phy_rd_start_i;
    unsigned ter(0..0)  phy_rd_valid_i;
    unsigned ter(0..63) phy_rd_data_i;

ST1_01:
    // Shared clock/reset
    INST_controller.CLK ::= CLK;
    INST_controller.rst ::= rst;
    INST_write_phy.CLK ::= CLK;
    INST_write_phy.rst ::= rst;
    INST_read_phy.CLK ::= CLK;
    INST_read_phy.rst ::= rst;

    // External request -> controller 
    INST_controller.req_valid ::= req_valid;
    INST_controller.req_write ::= req_write;
    INST_controller.req_addr ::= req_addr;
    INST_controller.req_wdata ::= req_wdata;
    INST_controller.req_wmask ::= req_wmask;

    // Controller response/status -> external 
    req_ready ::= INST_controller.req_ready;
    rsp_valid ::= INST_controller.rsp_valid;
    rsp_rdata ::= INST_controller.rsp_rdata;
    init_done ::= INST_controller.init_done;

    // Controller command/address -> DDR2
    ddr_cke ::= INST_controller.ddr_cke;
    ddr_cs_n ::= INST_controller.ddr_cs_n;
    ddr_ras_n ::= INST_controller.ddr_ras_n;
    ddr_cas_n ::= INST_controller.ddr_cas_n;
    ddr_we_n ::= INST_controller.ddr_we_n;
    ddr_odt ::= INST_controller.ddr_odt;
    ddr_ba ::= INST_controller.ddr_ba;
    ddr_addr ::= INST_controller.ddr_addr;

    // Controller -> mixed-edge write PHY 
    phy_wr_start_i ::= INST_controller.phy_wr_start;
    phy_wr_data_i ::= INST_controller.phy_wr_data;
    phy_wr_mask_i ::= INST_controller.phy_wr_mask;

    INST_write_phy.phy_wr_start ::= phy_wr_start_i;
    INST_write_phy.phy_wr_data ::= phy_wr_data_i;
    INST_write_phy.phy_wr_mask ::= phy_wr_mask_i;

    // Mixed-edge write PHY -> DDR2
    dq_out ::= INST_write_phy.dq_out;
    dm_out ::= INST_write_phy.dm_out;
    dqs_out ::= INST_write_phy.dqs_out;
    dq_oe ::= INST_write_phy.dq_oe;
    dqs_oe ::= INST_write_phy.dqs_oe;
    phy_wr_busy ::= INST_write_phy.phy_wr_busy;
    phy_wr_done ::= INST_write_phy.phy_wr_done;

    // Controller -> mixed-edge read PHY 
    phy_rd_start_i ::= INST_controller.phy_rd_start;
    INST_read_phy.phy_rd_start ::= phy_rd_start_i;
    INST_read_phy.dq_in ::= dq_in;

    // Mixed-edge read PHY -> controller
    phy_rd_valid_i ::= INST_read_phy.phy_rd_valid;
    phy_rd_data_i ::= INST_read_phy.phy_rd_data;

    INST_controller.phy_rd_valid ::= phy_rd_valid_i;
    INST_controller.phy_rd_data ::= phy_rd_data_i;

    goto ST1_01;
}

#else
#include <stdint.h>
#include "ddr2_controller.h"
#include "ddr2_write_phy.h"
#include "ddr2_read_phy.h"
#include "ddr2_top.h"

static ddr2_write_phy_t write_phy;
static ddr2_read_phy_t  read_phy;

void ddr2_top_init(void)
{
    ddr2_controller_init();
    ddr2_write_phy_init(&write_phy);
    ddr2_read_phy_init(&read_phy);

    phy_rd_valid = 0;
    phy_rd_data = 0;
}

void ddr2_top_rising(
    uint8_t rst_i,
    uint8_t req_valid_i,
    uint8_t req_write_i,
    uint32_t req_addr_i,
    uint64_t req_wdata_i,
    uint8_t req_wmask_i,
    uint16_t dq_in_i,
    ddr2_top_outputs_t *out)
{
    rst       = rst_i;
    req_valid = req_valid_i;
    req_write = req_write_i;
    req_addr  = req_addr_i;
    req_wdata = req_wdata_i;
    req_wmask = req_wmask_i;

    /* Present the previous read-PHY result to the controller. */
    phy_rd_valid = (uint8_t)read_phy.phy_rd_valid;
    phy_rd_data  = read_phy.phy_rd_data;

    ddr2_controller_step();

    ddr2_read_phy_edge(&read_phy, 1, phy_rd_start, dq_in_i);

    ddr2_write_phy_tick(&write_phy,
                        rst_i,
                        phy_wr_start,
                        phy_wr_data,
                        phy_wr_mask);
    ddr2_write_phy_eval(&write_phy, 1);

    out->req_ready   = req_ready;
    out->rsp_valid   = rsp_valid;
    out->rsp_rdata   = rsp_rdata;
    out->init_done   = init_done;
    out->phy_wr_start = phy_wr_start;
    out->phy_rd_start = phy_rd_start;

    out->dq_out      = write_phy.dq_out;
    out->dm_out      = write_phy.dm_out;
    out->dqs_out     = write_phy.dqs_out;
    out->dq_oe       = (uint8_t)write_phy.dq_oe;
    out->dqs_oe      = (uint8_t)write_phy.dqs_oe;
    out->phy_wr_busy = (uint8_t)write_phy.phy_wr_busy;
    out->phy_wr_done = (uint8_t)write_phy.phy_wr_done;
}

void ddr2_top_falling(
    uint8_t rst_i,
    uint16_t dq_in_i,
    ddr2_top_outputs_t *out)
{
    (void)rst_i;

    /* Controller state advances only on the rising CK edge. */
    ddr2_read_phy_edge(&read_phy, 0, 0, dq_in_i);
    ddr2_write_phy_eval(&write_phy, 0);

    out->dq_out      = write_phy.dq_out;
    out->dm_out      = write_phy.dm_out;
    out->dqs_out     = write_phy.dqs_out;
    out->dq_oe       = (uint8_t)write_phy.dq_oe;
    out->dqs_oe      = (uint8_t)write_phy.dqs_oe;
    out->phy_wr_busy = (uint8_t)write_phy.phy_wr_busy;
    out->phy_wr_done = (uint8_t)write_phy.phy_wr_done;
}

#endif
