# Archived waveform captures

These are real VCD captures supplied with the project, not screenshots or automatically generated outputs of the GitHub build.

| File | Origin | Observed evidence |
| --- | --- | --- |
| [`systemc_behavioral.vcd.gz`](systemc_behavioral.vcd.gz) | SystemC behavioral simulation (`trace_behav.vcd`) | Initialization; two request pulses; four-beat WRITE (`0x1111`, `0x2222`, `0x3333`, `0x4444`) and masks (`0`, `1`, `2`, `3`); `rsp_valid` with `rsp_rdata = 0x1111222233334444` |
| [`bdl_generated_rtl.vcd.gz`](bdl_generated_rtl.vcd.gz) | ModelSim simulation of BDL-generated RTL | Initialization, host request and write-path activity, plus two `rsp_valid` pulses at approximately 205,260 ns and 205,460 ns. |

Both use a **1 ps VCD timescale**. The SystemC trace ends at approximately **201,635 ns**; the RTL trace ends at approximately **225,202.5 ns**. Different schedules mean corresponding transactions should be compared by their external behavior, not their absolute timestamps.

## Open in a waveform viewer

```bash
gzip -dk systemc_behavioral.vcd.gz
gzip -dk bdl_generated_rtl.vcd.gz
gtkwave systemc_behavioral.vcd
gtkwave bdl_generated_rtl.vcd
```

In the RTL VCD, signals such as `dq_out`, `dm_out`, and `rsp_rdata` are represented as separate bit signals. Reconstruct them as buses in GTKWave or ModelSim. See [verification.md](../docs/verification.md) for event timestamps and the limits of the evidence.
