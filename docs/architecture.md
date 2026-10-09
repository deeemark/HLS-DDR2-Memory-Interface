# Architecture

## Functional partitions

**Controller:** accepts host requests and issues logical DDR2 command/address signals; source includes initialization, refresh, timing enforcement and request-handling logic. The precise timing rules and command coverage should be cross-referenced against code and directed tests before publishing a per-constraint PASS matrix.

**Write PHY model:** converts one 64-bit host write into four 16-bit transfers, with per-beat data-mask and DQS-related signaling.

**Read PHY model:** captures four 16-bit transfers and reconstructs the 64-bit response.

**SystemC:** `ddr2_top.cpp`, `ddr2_controller.cpp`, `ddr2_write_phy*.cpp`, `ddr2_read_phy*.cpp`; mixed-edge functions are split into positive/negative-edge modules.

**BDL/C:** `ddr2_top.c`, `ddr2_controller.c`, `ddr2_write_phy.c`, `ddr2_read_phy.c`.

## Scope

The PHY is a logical/behavioral datapath model.
