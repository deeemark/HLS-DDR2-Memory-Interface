# CyberWorkBench synthesis and generated-RTL verification

This guide distinguishes **behavioral C/SystemC testing**, **HLS synthesis**, and **generated-RTL simulation**.

## Prerequisites

- NEC CyberWorkBench 6.1 and an appropriate license.
- SystemC OSCI 2.3.0 installation used by the SystemC benchmark (`/proj/cad/cwb-6.1/osci` in the supplied Makefile).
- A supported RTL simulator such as the ModelSim environment used during development.
- The selected CyberWorkBench target technology/library.

## 1. Behavioral baselines

From `bdl_DDR2/`:

```bash
make clean
make
```

This compiles with `-DC_SIM` and executes the C behavioral integration test. **It does not invoke HLS synthesis.**

From `sysc_DDR2/`:

```bash
make clean
make
make wave
```

`make` executes the SystemC behavioral integration test; `make wave` rebuilds with waveform tracing enabled and creates `trace_behav.vcd`.

## 2. BDL-oriented C synthesis

The BDL-oriented sources are `ddr2_controller.c`, `ddr2_write_phy.c`, `ddr2_read_phy.c`, and `ddr2_top.c` (with their headers). The `C_SIM` branch is for GCC behavioral simulation; synthesis uses the BDL/default branch.

**Scheduling modes documented in the original BDL README:**

| Module            | Scheduling                     |
| ----------------- | ------------------------------ |
| `ddr2_controller` | Automatic scheduling supported |
| `ddr2_write_phy`  | Automatic scheduling supported |
| `ddr2_read_phy`   | Manual scheduling (`-sN`)      |
| `DDR2_TOP`        | Manual scheduling (`-sN`)      |

`DDR2_TOP` uses structural continuous assignments (`::=`), which are unsupported in automatic scheduling mode. The design was successful synthesized with a **5 ns target clock**.

In CyberWorkBench, add the corresponding synthesis sources/headers, configure the appropriate scheduling mode for each module, select the intended technology/library and 5 ns target, and synthesize. **Do not run `make` expecting synthesized RTL.** The exact shell synthesis commands are not included in the uploaded Makefiles, so none are asserted here.

## 3. SystemC synthesis

Use `sysc_DDR2/ddr2_top_cwb.cpp` as the **single aggregate synthesis input** to `scpars`. It includes the required implementation files in their intended order. The SystemC README explicitly warns not to supply those `.cpp` implementation files again as independent `scpars` inputs.

In the CyberWorkBench GUI, add `ddr2_top_cwb.cpp` as the synthesis source and place its referenced synthesizable implementation files in the included-source section. Configure the target library, clock constraint, and synthesis options, then save the resulting project configuration and reports. The behavioral testbench and `main.cpp` are simulation drivers, not separate top-level synthesis sources.

## 4. Generated RTL and ModelSim

The supplied `DDR2_TOP_E(2).vcd` is a generated-RTL waveform artifact, **not** the output of `bdl_DDR2/Makefile`.

For a reproducible RTL check:

1. Generate RTL for the selected top-level/module(s) in CyberWorkBench.
2. Use CWB's generated RTL testbench/stimulus flow and the same request/data sequence as the behavioral test.
3. Compile and simulate the generated RTL and testbench in the supported simulator (ModelSim was used during development).
4. Capture simulator pass/fail logs and a VCD of the command/address, data, DQS/DM, and request/response signals.
5. Compare **externally observable transactions** against the SystemC and C behavioral reference. HLS scheduling can change internal cycle alignment, so do not assert cycle-by-cycle equivalence without a specific measurement.
