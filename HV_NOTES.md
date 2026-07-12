# Hypervisor.framework Private APIs

Hypervisor.framework (HVF) has a number of private APIs required to enable
implementation-defined Apple Silicon extensions (such as SPRR/ GXF). This is a
WIP document/ brain dump about Hypervisor.framework as it relates to running
Apple Silicon custom instructions inside guest VMs.

TLDR: if you want SPRR/ GXF to be enabled inside your VM:

1. Set the ISA to 4 using `_hv_vm_config_set_isa`.
2. Unset control field 0 bit 20 using `_hv_vcpu_set_control_field`
3. Set control field 9 to -1 (again using `_hv_vcpu_set_control_field`)

## Entitlements

There are three relevant entitlements; one public and two private (in
increasing order of power):

- `com.apple.security.hypervisor`
- `com.apple.private.hypervisor.vmapple`
- `com.apple.private.hypervisor`

You should probably just give yourself all 3 of them. You can use
`entitlements.plist` in this repo:

```
codesign -s - --entitlements entitlements.plist $(YOUR_BINARY)
```

## ISA Mode

The `_hv_vm_config_set_isa` API lets you change which ISA you want your new VM
to use. There are currently 4 defined ISAs:

| ISA | Meaning                                                                      |
|-----|------------------------------------------------------------------------------|
|  1  | **Default** Simple ARM v8-A (nothing special)                                |
|  2  | ??                                                                           |
|  3  | VMAPPLE mode with new PAC stuff, but no SPRR/ GXF                            |
|  4  | Everything on- this is as close to a "real" bare metal Apple CPU as possible |

Use `_hv_vm_config_set_isa` to set the ISA mode for your `hv_vm_config_t`
before you use `hv_vm_create` to create it.

You almost definitely want ISA mode 4.

## Capabilities

`_hv_capability` is a read-only API that tells you info about some control regs
that affect your VM like `hcr_el2` and `hacr_el2`.

| IDX | Register                    |
|-----|-----------------------------|
|  2  | `HCR`                       |
|  3  | `CPTR`                      |
|  4  | `MDCR`                      |
|  7  | Timer                       |
|  9  | `HACR`                      |
| 12  | `APSTS`                     |
| 14  | `HFGRTR`                    |
| 15  | `HFGWTR`                    |
| 16  | `HFGITR`                    |
| 17  | `HDFGRTR`                   |
| 18  | Virtual interface GIC       |
| 19  | GIC NPIE active pending bug |
| 22  | `CNTHCTL`                   |

Everything else is undefined. Each one of these maps to a call to
`Arm::HypervisorCapabilities::get_*`. These are static and don't seem to be
updated if you change a corresponding control field.

For some of these (like `HACR`), the value originates in the kernel (from
`__hacr_cap_default`). If you patch your kernel you can change their values.

## Control Fields

Each vCPU has a number of "control fields" which can be adjusted with
`_hv_vcpu_set_control_field`. These let you override control registers like HCR
and HACR.

Known control fields:

| IDX | What it does                                   |
|-----|------------------------------------------------|
|  0  | `HCR_EL2`                                      |
|  2  | How to handle debug register / exception traps |
|  9  | `HACR_EL2` (when ISA = 4)                      |

## Context Buffer

This is a `0x4000` (or more?) byte buffer that contains everything related to
VM state.

It starts with the CPU registers (`+0` is literally X0, `+8` is X1, and so on).
The control fields themselves are also mapped in here somewhere(?), as are
SPRR/ GXF regs (for example, `sprr_config_el1` is at `+0x950`, see
`HvCore::Hypervisor::VcpuStateManager::set_sprr_config_el1`).

There is a mask field at `+0x788` that tracks SPRR/ GXF information. If SPRR is
accessed, bit 59 is set. If GXF is accessed, bit 58 is set.

You can retrieve the context buffer with `_hv_vcpu_get_context`.

This page is shared with the kernel, see `hv_vcpu_save_state_with_mask`.
