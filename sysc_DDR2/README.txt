===================================================================================
                           DDR2 SYSTEMC HLS BENCHMARK
===================================================================================

DDR2 SystemC HLS Benchmark is a SystemC benchmark implementing a DDR2 SDRAM
controller together with mixed-edge write and read PHY models. The design is
intended for behavioral simulation and high-level synthesis with NEC
CyberWorkBench.

The benchmark uses the following DDR2 configuration:

DDR type        : DDR2-400B
Data width      : x16
Banks           : 4
Burst length    : 4
Clock period    : 5 ns
Clock frequency : 200 MHz
Data rate       : 400 MT/s
CAS latency     : 3
Additive latency: 0
Read latency    : 3
Write latency   : 2


The benchmark contains the following files:

Makefile
--------
make       : Generates the DDR2 benchmark executable and runs the behavioral test
make wave  : Generates the executable with waveform tracing enabled, runs the
             behavioral test, and produces trace_behav.vcd
make clean : Cleans the executable, object files, and waveform file


SystemC files
-------------
ddr2_controller.cpp /.h : DDR2 initialization, command scheduling, refresh,
                          request handling, and timing enforcement

ddr2_write_phy.cpp /.h  : Top-level mixed-edge DDR2 write PHY integration
ddr2_write_phy_pos.cpp /.h : Positive-edge portion of the DDR2 write PHY
ddr2_write_phy_neg.cpp /.h : Negative-edge portion of the DDR2 write PHY

ddr2_read_phy.cpp /.h   : Top-level mixed-edge DDR2 read PHY integration
ddr2_read_phy_pos.cpp /.h : Positive-edge portion of the DDR2 read PHY
ddr2_read_phy_neg.cpp /.h : Negative-edge portion of the DDR2 read PHY

ddr2_top.cpp /.h        : Top-level integration of the DDR2 controller,
                          write PHY, and read PHY


Main/Testbench files
--------------------
main.cpp          : Top-level SystemC simulation setup, signal connections,
                    reset, waveform tracing, and simulation control

tb_ddr2_top.cpp /.h : Testbench for the complete integrated DDR2 design


CyberWorkBench file
-------------------
ddr2_top_cwb.cpp : Aggregate CyberWorkBench synthesis input. This file includes
                   the required SystemC implementation files in the proper order.

Only ddr2_top_cwb.cpp should be passed to scpars. The individual implementation
.cpp files should not also be supplied as separate scpars inputs. When using the gui,
only ddr2_top_cwb.cpp should be added as the source and the other synthesizable files
should be put in the included section.


The behavioral test checks the complete integrated DDR2 design. The test performs
a DDR2 WRITE followed by a DDR2 READ and verifies the x16 write serialization,
data masks, mixed-edge read capture, and 64-bit read reconstruction.

Example 64-bit transaction:

4444333322221111

DDR x16 burst order:

1111 -> 2222 -> 3333 -> 4444

Write data-mask sequence used by the test:

0 -> 1 -> 2 -> 3


The mixed-edge PHY is divided into separate positive-edge and negative-edge
SystemC modules. This organization allows CyberWorkBench to preserve the
required positive-edge and negative-edge behavior in the generated RTL.

The synthesized top-level module structurally connects the following major
components:

ddr2_controller
ddr2_write_phy
ddr2_read_phy


