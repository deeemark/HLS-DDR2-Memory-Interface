
This DDR2 HLS Benchmark is a BDL benchmark implementing a DDR2 Memory interface covering the DDR2 SDRAM controller
together with mixed-edge write and read PHY models. The design is intended for
behavioral simulation and high-level synthesis with NEC CyberWorkBench.

The DDR2 controller and write PHY may be synthesized using automatic scheduling.

The DDR2 read PHY and the structural DDR2_TOP module should be synthesized using
manual scheduling (-sN).

DDR2_TOP must use manual scheduling because the structural
continuous assignments (::=) are not supported in automatic scheduling mode.

All modules were verified to synthesize successfully with a 5 ns clock period.

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
make clean : Cleans the executable file

C/BDL files
-----------
ddr2_controller.c /.h : DDR2 initialization, command scheduling, refresh,
                        request handling, and timing enforcement

ddr2_write_phy.c /.h  : Mixed-edge DDR2 write PHY. Serializes a 64-bit
                        transaction into four x16 DDR data beats

ddr2_read_phy.c /.h   : Mixed-edge DDR2 read PHY. Captures four x16 DDR data
                        beats and reconstructs a 64-bit transaction

ddr2_top.c /.h        : Top-level integration of the DDR2 controller,
                        write PHY, and read PHY

Main/Testbench files
--------------------
main.c        : Main program for running the DDR2 benchmark
tb_ddr2_top.c : Testbench for the integrated DDR2 design

Stimuli files (.txt)
--------------------
ddr2_input.txt          : Input stimuli for the top-level DDR2 benchmark
ddr2_output_golden.txt  : Golden output with which the top-level simulation
                          results are compared


The behavioral test checks the complete integrated DDR2 design. The top-level
test uses a 64-bit DDR2 burst transaction and verifies the x16 DDR write
serialization and read reconstruction.

Example 64-bit transaction:

4444333322221111

DDR x16 burst order:

1111 -> 2222 -> 3333 -> 4444


The BDL portions of the source are used for synthesis with CyberWorkBench.
BDL is the default source path; no BDL preprocessor definition is required.
The Makefile defines C_SIM for the normal C behavioral build.
The synthesized top-level module is DDR2_TOP and structurally connects the
following lower-level modules:

ddr2_controller
ddr2_write_phy
ddr2_read_phy




