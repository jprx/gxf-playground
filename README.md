# HVF-GXF Playground

Using Hypervisor.framework to mess around with GXF / SPRR instructions and
MSRs.

Requirements:
1. Get `aarch64-elf` cross compiler (https://github.com/jprx/osdev-tools)
2. SIP disabled (`csrutil disable`) and research guests enabled (`csrutil allow-research-guests enable`).

Build & run:
1. `make`
2. `./hvf_gxf`

# Expected Output

```
[✅] 0: enable_sprr (Enable SPRR)
[✅] 1: enable_gxf (Enable GXF)
[✅] 2: gxf_without_sprr (GXF crashes without SPRR enabled first)
[✅] 3: vmtest (Virtual memory)
[✅] 4: simple_genter (Enter GL1)
[✅] 5: gxf_sp (Does GL1 use a different stack pointer than EL1?)

    SPRR Permissions
╭─────┬───────┬───────╮
│     │  GL1  │  EL1  │
│     │ R W X │ R W X │
├─────┼───────┼───────┤
│  0  │ 0 0 0 │ 0 0 0 │
│  1  │ 0 0 0 │ 1 0 1 │
│  2  │ 0 0 0 │ 1 0 0 │
│  3  │ 0 0 0 │ 1 1 0 │
│  4  │ 1 0 1 │ 0 0 0 │
│  5  │ 1 0 1 │ 1 0 1 │
│  6  │ 1 0 1 │ 1 0 0 │
│  7  │ 1 0 1 │ 0 0 0 │
│  8  │ 1 0 0 │ 0 0 0 │
│  9  │ 1 0 0 │ 0 0 1 │
│  A  │ 1 0 0 │ 1 0 0 │
│  B  │ 1 0 0 │ 1 1 0 │
│  C  │ 1 1 0 │ 0 0 0 │
│  D  │ 1 1 0 │ 1 0 1 │
│  E  │ 1 1 0 │ 1 0 0 │
│  F  │ 1 1 0 │ 1 1 0 │
╰─────┴───────┴───────╯
[✅] 6: sprr_test (Identify SPRR bit meanings)
7 of 7 tests pass
```

Notes
- cpsr follows spsr encoding; use this to set pstate bits (ofc cpsr isn't a real reg in aarch64)
- Need to set EL in cpsr before running first instruction
- Need to request HVF to trap debug exceptions with hv_vcpu_set_trap_debug_exceptions (see qemu hvf.c)
- ISA 4 has SPRR/ GXF, but needs HCR_EL2.TIDCP set to 0, and HACR_EL2 set to 0xFFFFFFFFFFFFFFFF to work (this disables trapping of implementation defined features)

## Credits

Thanks to never_released and Siguza for helping me with reversing
Hypervisor.framework and getting SPRR/ GXF instructions to run in a VM!
