# Verification and captured waveform evidence

This page distinguishes **testbench coverage** from **events visible in the archived VCD captures**. Both captures use a 1 ps timescale. The two simulations have different schedules and are not expected to align cycle-for-cycle.

## Testbench coverage

The included SystemC and BDL/C integration testbenches contain checks for initialization, 64-bit WRITE serialization into four 16-bit beats, write masks, and READ data reconstruction. The test stimulus includes the write word `0x4444333322221111` and mask progression `0, 1, 2, 3`. These checks establish intended test coverage; a testbench check is not itself proof that every supplied waveform contains the corresponding completed transaction.

## Events observed in the supplied SystemC VCD

Artifact: [`../waveforms/systemc_behavioral.vcd.gz`](../waveforms/systemc_behavioral.vcd.gz), originating from `trace_behav.vcd`.

| Signal/event                 |    Time in capture | Observation                                                                     |
| ---------------------------- | -----------------: | ------------------------------------------------------------------------------- |
| `init_done` rises            |         201,485 ns | Initialization complete                                                         |
| First `req_valid` assertion  |         201,486 ns | First host request                                                              |
| `dq_out` burst activity      | 201,525–201,535 ns | Four successive 16-bit values `0x1111`, `0x2222`, `0x3333`, `0x4444`, then zero |
| `dm_out` burst activity      | 201,525–201,535 ns | Mask values `0`, `1`, `2`, `3`, then zero                                       |
| Second `req_valid` assertion |       201,565.1 ns | Second host request                                                             |
| `rsp_valid` rises            |         201,635 ns | Read response asserted                                                          |
| `rsp_rdata` updates          |         201,635 ns | `0x1111222233334444` visible at response                                        |

The read response value above is the value **actually present on the captured `rsp_rdata` bus**. It should not be conflated with the earlier write stimulus `0x4444333322221111`; their word ordering differs. The source/testbench must be consulted to explain expected read input data.

## Events observed in the supplied BDL-generated RTL VCD

Artifact: [`../waveforms/bdl_generated_rtl.vcd.gz`](../waveforms/bdl_generated_rtl.vcd.gz). This is the selected, more complete ModelSim capture of BDL-generated RTL, archived under the stable repository filename `bdl_generated_rtl.vcd.gz`.

| Signal/event                 |           Time in capture | Observation                          |
| ---------------------------- | ------------------------: | ------------------------------------ |
| `init_done`                  |         Around 201,675 ns | Initialization completes             |
| `req_valid`                  |         Around 205,200 ns | Host request activity                |
| `dq_out` / `dm_out`          | Around 205,230–205,250 ns | Write datapath and mask activity     |
| `rsp_valid` first assertion  |                205,260 ns | Read response-valid pulse            |
| `rsp_valid` second assertion |                205,460 ns | Additional read response-valid pulse |

The VCD uses scalar bit signals for some buses, which can be grouped in a waveform viewer to inspect `rsp_rdata` and the DDR2 command/data buses.

## Viewing the archived captures

```bash
cd waveforms
gzip -dk systemc_behavioral.vcd.gz
gzip -dk bdl_generated_rtl.vcd.gz
gtkwave systemc_behavioral.vcd
gtkwave bdl_generated_rtl.vcd
```

For the RTL capture, group the scalar `dq_out[0]`–`dq_out[15]`, `dm_out[0]`–`dm_out[1]`, and `rsp_rdata[0]`–`rsp_rdata[63]` signals as buses in the viewer. For the SystemC capture, inspect `req_valid`, `req_ready`, `init_done`, command pins, `dq_out`, `dm_out`, `dq_in`, `rsp_valid`, and `rsp_rdata`.

## Reproducing behavioral tests

From `bdl_DDR2/`:

```bash
make clean
make
```

From `sysc_DDR2/`:

```bash
make clean
make
make wave
```

The BDL/C `make` runs the C behavioral test, **not** the generated RTL simulation. The SystemC `make wave` target generates `trace_behav.vcd`. For HLS and RTL details, see [synthesis-and-rtl.md](synthesis-and-rtl.md).
