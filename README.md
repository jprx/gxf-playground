# HVF-GXF Playground

Using Hypervisor.framework to mess around with GXF / SPRR instructions and
MSRs.

Requirements:
1. Get `aarch64-elf` cross compiler (https://github.com/jprx/osdev-tools)
2. SIP disabled (`csrutil disable`) and research guests enabled (`csrutil allow-research-guests enable`).

Build & run:
1. `make`
2. `./hvf_gxf`

Notes
- cpsr follows spsr encoding; use this to set pstate bits (ofc cpsr isn't a real reg in aarch64)
- Need to set EL in cpsr before running first instruction
- Need to request HVF to trap debug exceptions with hv_vcpu_set_trap_debug_exceptions (see qemu hvf.c)
- ISA 4 has SPRR/ GXF, but needs HCR_EL2.TIDCP set to 0, and HACR_EL2 set to 0xFFFFFFFFFFFFFFFF to work (this disables trapping of implementation defined features)

## Credits

Thanks to never_released and Siguza for helping me with reversing
Hypervisor.framework and getting SPRR/ GXF instructions to run in a VM!
