# HVF-GXF Playground

Using Hypervisor.framework to mess around with GXF / SPRR instructions and
MSRs.

Requirements:
1. Get `aarch64-elf` cross compiler (https://github.com/jprx/osdev-tools)
2. SIP disabled (`csrutil disable`)
3. Research guests enabled(?) (`csrutil allow-research-guests enable`)

Build & run:
1. `make`
2. `./hvf_gxf`

Notes
- cpsr follows spsr encoding; use this to set pstate bits (ofc cpsr isn't a real reg in aarch64)
- Need to set EL in cpsr before running first instruction
- Need to request HVF to trap debug exceptions with hv_vcpu_set_trap_debug_exceptions (see qemu hvf.c)
