#pragma once
#include <Hypervisor/Hypervisor.h>
#include <stdint.h>

typedef struct {
  uint64_t  x0;
  uint64_t  x1;
  uint64_t  x2;
  uint64_t  x3;
  uint64_t  x4;
  uint64_t  x5;
  uint64_t  x6;
  uint64_t  x7;
  uint64_t  x8;
  uint64_t  x9;
  uint64_t  x10;
  uint64_t  x11;
  uint64_t  x12;
  uint64_t  x13;
  uint64_t  x14;
  uint64_t  x15;
  uint64_t  x16;
  uint64_t  x17;
  uint64_t  x18;
  uint64_t  x19;
  uint64_t  x20;
  uint64_t  x21;
  uint64_t  x22;
  uint64_t  x23;
  uint64_t  x24;
  uint64_t  x25;
  uint64_t  x26;
  uint64_t  x27;
  uint64_t  x28;
  uint64_t  fp;
  uint64_t  lr;
  uint64_t  pc;
  uint64_t  fpcr;
  uint64_t  fpsr;
  uint64_t  cpsr;
  uint64_t  elr;
  uint64_t  far;
  uint64_t  esr;
  uint64_t  sp_el0;
  uint64_t  sp_el1;

  uint64_t  sprr_config_el1;
  uint64_t  sprr_amrange_el1;
  uint64_t  sprr_pperm_el1;
  uint64_t  sprr_pperm_el2;
  uint64_t  sprr_uperm_el0;
  uint64_t  sprr_pmprr_el1;
  uint64_t  sprr_umprr_el1;
  uint64_t  sprr_pperm_sh01_el1;
  uint64_t  sprr_pperm_sh02_el1;
  uint64_t  sprr_pperm_sh03_el1;
  uint64_t  sprr_pperm_sh04_el1;
  uint64_t  sprr_pperm_sh05_el1;
  uint64_t  sprr_pperm_sh06_el1;
  uint64_t  sprr_pperm_sh07_el1;
  uint64_t  sprr_uperm_sh01_el1;
  uint64_t  sprr_uperm_sh02_el1;
  uint64_t  sprr_uperm_sh03_el1;
  uint64_t  sprr_uperm_sh04_el1;
  uint64_t  sprr_uperm_sh05_el1;
  uint64_t  sprr_uperm_sh06_el1;
  uint64_t  sprr_uperm_sh07_el1;

  uint64_t  gxf_config_el1;
  uint64_t  gxf_entry_el1;
  uint64_t  gxf_pabentry_el1;

  uint64_t  sp_gl1;
  uint64_t  tpidr_gl1;
  uint64_t  aspsr_gl1;
  uint64_t  vbar_gl1;
  uint64_t  far_gl1;
  uint64_t  esr_gl1;
  uint64_t  elr_gl1;
  uint64_t  spsr_gl1;
  uint64_t  pmcr1_gl1;
  uint64_t  afsr1_gl1;
} regs_t;

void get_regs(hv_vcpu_t cpu, regs_t *regs);
void dump_regs(regs_t *r);
