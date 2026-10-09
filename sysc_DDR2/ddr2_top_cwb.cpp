/*
 * Single CWB 6.1/scpars synthesis input for the complete DDR2 design.
 * Do NOT pass the individual implementation .cpp files to scpars as
 * additional inputs.  CWB 6.1 does not support separate SystemC compilation.
 */

// Lowest-level mixed-edge PHY modules first.
#include "ddr2_write_phy_pos.cpp"
#include "ddr2_write_phy_neg.cpp"
#include "ddr2_write_phy.cpp"

#include "ddr2_read_phy_pos.cpp"
#include "ddr2_read_phy_neg.cpp"
#include "ddr2_read_phy.cpp"

// Controller and structural parent last.
#include "ddr2_controller.cpp"
#include "ddr2_top.cpp"
