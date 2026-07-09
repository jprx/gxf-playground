#include <stdio.h>
#include <inttypes.h>
#include "regs.h"

void get_regs(hv_vcpu_t cpu, regs_t *r) {
  hv_vcpu_get_reg(cpu, HV_REG_X0,   &r->x0);
  hv_vcpu_get_reg(cpu, HV_REG_X1,   &r->x1);
  hv_vcpu_get_reg(cpu, HV_REG_X2,   &r->x2);
  hv_vcpu_get_reg(cpu, HV_REG_X3,   &r->x3);
  hv_vcpu_get_reg(cpu, HV_REG_X4,   &r->x4);
  hv_vcpu_get_reg(cpu, HV_REG_X5,   &r->x5);
  hv_vcpu_get_reg(cpu, HV_REG_X6,   &r->x6);
  hv_vcpu_get_reg(cpu, HV_REG_X7,   &r->x7);
  hv_vcpu_get_reg(cpu, HV_REG_X8,   &r->x8);
  hv_vcpu_get_reg(cpu, HV_REG_X9,   &r->x9);
  hv_vcpu_get_reg(cpu, HV_REG_X10,  &r->x10);
  hv_vcpu_get_reg(cpu, HV_REG_X11,  &r->x11);
  hv_vcpu_get_reg(cpu, HV_REG_X12,  &r->x12);
  hv_vcpu_get_reg(cpu, HV_REG_X13,  &r->x13);
  hv_vcpu_get_reg(cpu, HV_REG_X14,  &r->x14);
  hv_vcpu_get_reg(cpu, HV_REG_X15,  &r->x15);
  hv_vcpu_get_reg(cpu, HV_REG_X16,  &r->x16);
  hv_vcpu_get_reg(cpu, HV_REG_X17,  &r->x17);
  hv_vcpu_get_reg(cpu, HV_REG_X18,  &r->x18);
  hv_vcpu_get_reg(cpu, HV_REG_X19,  &r->x19);
  hv_vcpu_get_reg(cpu, HV_REG_X20,  &r->x20);
  hv_vcpu_get_reg(cpu, HV_REG_X21,  &r->x21);
  hv_vcpu_get_reg(cpu, HV_REG_X22,  &r->x22);
  hv_vcpu_get_reg(cpu, HV_REG_X23,  &r->x23);
  hv_vcpu_get_reg(cpu, HV_REG_X24,  &r->x24);
  hv_vcpu_get_reg(cpu, HV_REG_X25,  &r->x25);
  hv_vcpu_get_reg(cpu, HV_REG_X26,  &r->x26);
  hv_vcpu_get_reg(cpu, HV_REG_X27,  &r->x27);
  hv_vcpu_get_reg(cpu, HV_REG_X28,  &r->x28);
  hv_vcpu_get_reg(cpu, HV_REG_FP,   &r->fp);
  hv_vcpu_get_reg(cpu, HV_REG_LR,   &r->lr);
  hv_vcpu_get_reg(cpu, HV_REG_PC,   &r->pc);
  hv_vcpu_get_reg(cpu, HV_REG_FPCR, &r->fpcr);
  hv_vcpu_get_reg(cpu, HV_REG_FPSR, &r->fpsr);
  hv_vcpu_get_reg(cpu, HV_REG_CPSR, &r->cpsr);

  hv_vcpu_get_sys_reg(cpu, HV_SYS_REG_ELR_EL1, &r->elr);
  hv_vcpu_get_sys_reg(cpu, HV_SYS_REG_FAR_EL1, &r->far);
  hv_vcpu_get_sys_reg(cpu, HV_SYS_REG_ESR_EL1, &r->esr);
  hv_vcpu_get_sys_reg(cpu, HV_SYS_REG_SP_EL0,  &r->sp_el0);
  hv_vcpu_get_sys_reg(cpu, HV_SYS_REG_SP_EL1,  &r->sp_el1);
}

void dump_regs(regs_t *r) {
  printf("========\n");
  printf("PC:   0x%016llX\n",      r->pc);
  printf("X0:   0x%016llX     ",   r->x0);
  printf("X1:   0x%016llX     ",   r->x1);
  printf("X2:   0x%016llX     ",   r->x2);
  printf("X3:   0x%016llX\n",      r->x3);
  printf("ELR:  0x%016llX     ",   r->elr);
  printf("CPSR: 0x%016llX     ",   r->cpsr);
  printf("FAR:  0x%016llX     ",   r->far);
  printf("ESR:  0x%016llX\n",      r->esr);
#ifdef DUMP_ALL
  printf("X4:  0x%016llX     ", r->x4);
  printf("X5:  0x%016llX     ", r->x5);
  printf("X6:  0x%016llX     ", r->x6);
  printf("X7:  0x%016llX\n",    r->x7);
  printf("X8:  0x%016llX     ", r->x8);
  printf("X9:  0x%016llX     ", r->x9);
  printf("X10: 0x%016llX     ", r->x10);
  printf("X11: 0x%016llX\n",    r->x11);
  printf("X12: 0x%016llX     ", r->x12);
  printf("X13: 0x%016llX     ", r->x13);
  printf("X14: 0x%016llX     ", r->x14);
  printf("X15: 0x%016llX\n",    r->x15);
  printf("X16: 0x%016llX     ", r->x16);
  printf("X17: 0x%016llX     ", r->x17);
  printf("X18: 0x%016llX     ", r->x18);
  printf("X19: 0x%016llX\n",    r->x19);
  printf("X20: 0x%016llX     ", r->x20);
  printf("X21: 0x%016llX     ", r->x21);
  printf("X22: 0x%016llX     ", r->x22);
  printf("X23: 0x%016llX\n",    r->x23);
  printf("X24: 0x%016llX     ", r->x24);
  printf("X25: 0x%016llX     ", r->x25);
  printf("X26: 0x%016llX     ", r->x26);
  printf("X27: 0x%016llX\n",    r->x27);
#endif // DUMP_ALL
}
