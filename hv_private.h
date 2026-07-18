#pragma once
#include <Hypervisor/Hypervisor.h>

#define ISA_PRIVATE            4

// Hypervisor.framework's hv_sys_reg enum type is a uint16_t that packs the 5 MSR fields into one u16:
#define PACK_SYSREG(op0,op1,crn,crm,op2) (( (((op0)) << 14) | (((op1)) << 11) | (((crn)) << 7) | (((crm)) << 3) | ((op2)) ))

#define  HV_SYS_REG_SPRR_CONFIG_EL1          PACK_SYSREG(3,  6,  15,  1,   0)
#define  HV_SYS_REG_SPRR_AMRANGE_EL1         PACK_SYSREG(3,  6,  15,  1,   3)
#define  HV_SYS_REG_SPRR_PPERM_EL1           PACK_SYSREG(3,  6,  15,  1,   6)
#define  HV_SYS_REG_SPRR_PPERM_EL2           PACK_SYSREG(3,  6,  15,  1,   7)
#define  HV_SYS_REG_SPRR_UPERM_EL0           PACK_SYSREG(3,  6,  15,  1,   5)
#define  HV_SYS_REG_SPRR_PMPRR_EL1           PACK_SYSREG(3,  6,  15,  3,   1)
#define  HV_SYS_REG_SPRR_UMPRR_EL1           PACK_SYSREG(3,  6,  15,  3,   0)
#define  HV_SYS_REG_SPRR_PPERM_SH01_EL1      PACK_SYSREG(3,  6,  15,  4,   2)
#define  HV_SYS_REG_SPRR_PPERM_SH02_EL1      PACK_SYSREG(3,  6,  15,  4,   3)
#define  HV_SYS_REG_SPRR_PPERM_SH03_EL1      PACK_SYSREG(3,  6,  15,  4,   4)
#define  HV_SYS_REG_SPRR_PPERM_SH04_EL1      PACK_SYSREG(3,  6,  15,  4,   5)
#define  HV_SYS_REG_SPRR_PPERM_SH05_EL1      PACK_SYSREG(3,  6,  15,  4,   6)
#define  HV_SYS_REG_SPRR_PPERM_SH06_EL1      PACK_SYSREG(3,  6,  15,  4,   7)
#define  HV_SYS_REG_SPRR_PPERM_SH07_EL1      PACK_SYSREG(3,  6,  15,  5,   0)
#define  HV_SYS_REG_SPRR_UPERM_SH01_EL1      PACK_SYSREG(3,  6,  15,  3,   3)
#define  HV_SYS_REG_SPRR_UPERM_SH02_EL1      PACK_SYSREG(3,  6,  15,  3,   4)
#define  HV_SYS_REG_SPRR_UPERM_SH03_EL1      PACK_SYSREG(3,  6,  15,  3,   5)
#define  HV_SYS_REG_SPRR_UPERM_SH04_EL1      PACK_SYSREG(3,  6,  15,  3,   6)
#define  HV_SYS_REG_SPRR_UPERM_SH05_EL1      PACK_SYSREG(3,  6,  15,  3,   7)
#define  HV_SYS_REG_SPRR_UPERM_SH06_EL1      PACK_SYSREG(3,  6,  15,  4,   0)
#define  HV_SYS_REG_SPRR_UPERM_SH07_EL1      PACK_SYSREG(3,  6,  15,  4,   1)

#define  HV_SYS_REG_GXF_CONFIG_EL1           PACK_SYSREG(3,  6,  15,  1,   2)
#define  HV_SYS_REG_GXF_ENTRY_EL1            PACK_SYSREG(3,  6,  15,  8,   1)
#define  HV_SYS_REG_GXF_PABENTRY_EL1         PACK_SYSREG(3,  6,  15,  8,   2)

#define  HV_SYS_REG_SP_GL1                   PACK_SYSREG(3,  6,  15,  10,  0)
#define  HV_SYS_REG_TPIDR_GL1                PACK_SYSREG(3,  6,  15,  10,  1)
#define  HV_SYS_REG_ASPSR_GL1                PACK_SYSREG(3,  6,  15,  10,  4)
#define  HV_SYS_REG_VBAR_GL1                 PACK_SYSREG(3,  6,  15,  10,  2)
#define  HV_SYS_REG_FAR_GL1                  PACK_SYSREG(3,  6,  15,  10,  7)
#define  HV_SYS_REG_ESR_GL1                  PACK_SYSREG(3,  6,  15,  10,  5)
#define  HV_SYS_REG_ELR_GL1                  PACK_SYSREG(3,  6,  15,  10,  6)
#define  HV_SYS_REG_SPSR_GL1                 PACK_SYSREG(3,  6,  15,  10,  3)
#define  HV_SYS_REG_PMCR1_GL1                PACK_SYSREG(3,  6,  15,  8,   7)
#define  HV_SYS_REG_AFSR1_GL1                PACK_SYSREG(3,  6,  15,  0,   1)

enum {
  HV_CTRL_HCR_EL2          =  0,
  HV_CTRL_TRAP_DEBUG_MODE  =  2, // 0x100 = trap debug exceptions, 0x200 = trap debug registers
  HV_CTRL_HACR_EL2         =  9,
};

extern uint64_t     _hv_vm_config_get_isa(hv_vm_config_t);
extern hv_return_t  _hv_vm_config_set_isa(hv_vm_config_t, uint64_t);
extern hv_return_t  _hv_vcpu_set_control_field(hv_vcpu_t, uint64_t, uint64_t);
extern hv_return_t  _hv_vcpu_get_control_field(hv_vcpu_t, uint64_t, uint64_t*);
extern void        *_hv_vcpu_get_context(hv_vcpu_t);
extern void         _hv_capability(uint32_t, int64_t *);
