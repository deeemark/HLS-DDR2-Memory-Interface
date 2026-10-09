# DDR2 Memory Interface — SystemC and BDL

A DDR2-400B interface benchmark implemented in **SystemC** and **C/BDL** for behavioral verification and high-level synthesis with NEC CyberWorkBench 6.1. The design combines command/control logic with logical read and write PHY datapaths.

## Configuration

| Parameter                      | Value                                    |
| ------------------------------ | ---------------------------------------- |
| DRAM organization              | DDR2-400B, x16, 4 banks                  |
| Clock / data rate              | 200 MHz / 400 MT/s                       |
| Burst length                   | 4 (four 16-bit beats per 64-bit request) |
| CAS / additive / write latency | 3 / 0 / 2 cycles                         |
| Host data / mask               | 64 bits / 8 bits                         |

## Architecture

```text
Host request (address, read/write, 64-bit data, mask)
                         |
                  DDR2 controller
           (initialization, commands,
            bank/timing management)
                  /           \
           Write PHY        Read PHY
         64 -> 4x16        4x16 -> 64
                  \           /
          DDR2 logical command/data interface
             (CS/RAS/CAS/WE, BA, A,
                 DQ, DQS, DM)
```

SystemC includes positive- and negative-edge read/write PHY modules. BDL/C contains controller, write PHY, read PHY, and top-level source files. See [architecture](docs/architecture.md).

## Verification

The included integration testbenches check initialization, a WRITE with four serialized x16 beats and data masks, and a READ reconstructed into a 64-bit response. The representative write word is `0x4444333322221111`, serialized least-significant 16-bit beat first (`1111`, `2222`, `3333`, `4444`). See [verification](docs/verification.md) for the measured events in the archived traces and instructions for viewing them.

**Waveform evidence:** The archived SystemC trace shows initialization, four write beats/masks, and a completed read response (`rsp_rdata = 0x1111222233334444`). The archived BDL-generated RTL trace shows initialization and write-path activity, but its top-level `rsp_valid` does not assert. These observations are documented separately from testbench coverage; they do not establish full DDR2 timing coverage or physical timing closure.

## Build, run, and generate waveforms

Run each command from its implementation directory. Both `make` targets **compile and execute** their integrated behavioral testbench; they do not just build an executable.

### BDL/C behavioral test (`bdl_DDR2/`)

```bash
cd bdl_DDR2
make clean
make
```

- `make clean` removes `ddr2_test`.
- `make` compiles the C behavioral simulation using GCC, `-DC_SIM`, and C99, then runs `./ddr2_test`.
- This is the **C behavioral mode** of the BDL-oriented source, not the CyberWorkBench RTL synthesis command. The synthesis workflow uses the BDL branches of the source and CyberWorkBench separately.

### SystemC behavioral test (`sysc_DDR2/`)

```bash
cd sysc_DDR2
make clean
make
make wave
```

- `make clean` removes the executable, object files, and any prior `trace_behav.vcd`.
- `make` builds `ddr2_top_test` with SystemC and runs it.
- `make wave` cleans, rebuilds with `-DWAVE_DUMP`, reruns the test, and produces `trace_behav.vcd`.
- `make run` is also defined, but is an alias of the normal `make` build-and-run target.
- The Makefile expects SystemC at `/proj/cad/cwb-6.1/osci`; change `SYSTEMC` if your installation differs.

The source archive uses the folder names `bdl_DDR2` and `sysc_DDR2`. If you reorganize the GitHub repository, run these commands from the corresponding renamed folders.

## CyberWorkBench synthesis and RTL simulation

The BDL/C and SystemC `make` targets run **behavioral** tests. HLS synthesis and ModelSim generated-RTL verification are separate steps. See [CyberWorkBench synthesis and RTL verification](docs/synthesis-and-rtl.md) for the documented BDL scheduling requirements, the SystemC `ddr2_top_cwb.cpp` synthesis entry point, and the RTL evidence workflow.

## Captured waveforms

Compressed VCD captures from both implementations are available in [waveforms/](waveforms/README.md). The accompanying guide includes actual event timestamps. The selected BDL RTL capture includes two read-response-valid assertions, alongside initialization and write-path activity. They can be viewed without CyberWorkBench or ModelSim using a VCD viewer such as GTKWave.

## Validation history

The DDR2 integration transactions were previously exercised and reviewed during development, including initialization, burst WRITE serialization/masking, and READ reconstruction. The source includes PASS/FAIL checks and supplied SystemC and generated-RTL VCDs.

## Current limitations

- Behavioral/logical PHY modeling does not establish physical DDR2 I/O timing, training, or FPGA board operation.
- This repository does not claim arbitration or multi-request buffering.
- Published area, achieved frequency, and latency numbers should be added only after verifying the synthesis reports and measurement definitions.

## Repository contents

- [`sysc_DDR2/`](sysc_DDR2/) — SystemC sources, testbench, and `make wave` target.
- [`bdl_DDR2/`](bdl_DDR2/) — C/BDL sources, behavioral testbench, and Makefile.
- [`docs/`](docs/) — architecture, verification, and synthesis/RTL notes.
- [`waveforms/`](waveforms/) — compressed behavioral and RTL waveform captures.

## Copyright and licensing

Copyright © 2026 DeMarkus Taylor. No open-source license has been selected for this repository. All rights are reserved unless a license is added.
