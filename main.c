#include <Hypervisor/Hypervisor.h>
#include <stdio.h>
#include "aarch64.h"
#include "test.h"
#include "regs.h"
#include "hvf_utils.h"
#include "hv_private.h"

// RAN_FOR(regs, num):
// Returns true if the exit PC is num instructions away from the start
#define RAN_FOR(r,n)           (( (TEST_PHYS + (4 * ((n)))) == ((r.pc)) ))

// HIT_TRAP(regs)
// Returns true if we hit a trap to VBAR (+0x200 because we are using SP_EL1 by default)
#define HIT_TRAP(r)            (( ((VBAR_PHYS + 0x200) == ((r.pc))) || ((VBAR_VIRT + 0x200) == ((r.pc))) ))

void enable_sprr_handler(test_t *t);
void enable_gxf_handler(test_t *t);
void gxf_without_sprr_handler(test_t *t);
void vmtest_handler(test_t *t);
void simple_genter_handler(test_t *t);
void gxf_sp_handler(test_t *t);
void sprr_handler(test_t *t);

test_t tests[] = {
  {
    .name = "enable_sprr",
    .desc = "Enable SPRR",
    .handler = enable_sprr_handler,
  },
  {
    .name = "enable_gxf",
    .desc = "Enable GXF",
    .handler = enable_gxf_handler,
  },
  {
    .name = "gxf_without_sprr",
    .desc = "GXF crashes without SPRR enabled first",
    .handler = gxf_without_sprr_handler,
  },
  {
    .name = "vmtest",
    .desc = "Virtual memory",
    .handler = vmtest_handler,
    .needs_mmu = true,
  },
  {
    .name = "simple_genter",
    .desc = "Enter GL1",
    .handler = simple_genter_handler,
    .needs_mmu = true,
  },
  {
    .name = "gxf_sp",
    .desc = "Does GL1 use a different stack pointer than EL1?",
    .handler = gxf_sp_handler,
    .needs_mmu = true,
  },
  {
    .name = "sprr_test",
    .desc = "Identify SPRR bit meanings",
    .handler = sprr_handler,
    .needs_mmu = true,
  },
};

void enable_sprr_handler(test_t *t) {
  regs_t r;
  setup_vm(t, t->name);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);
  t->test_pass = RAN_FOR(r,2) && (1 == r.sprr_config_el1);
  destroy_vm(t);
}

void enable_gxf_handler(test_t *t) {
  regs_t r;
  setup_vm(t, t->name);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);
  t->test_pass = RAN_FOR(r,3) && (1 == r.sprr_config_el1) && (1 == r.gxf_config_el1);
  destroy_vm(t);
}

void gxf_without_sprr_handler(test_t *t) {
  regs_t r;
  setup_vm(t, t->name);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);
  t->test_pass = HIT_TRAP(r) && (0 == r.gxf_config_el1);
  destroy_vm(t);
}

void vmtest_handler(test_t *t) {
  regs_t r;
  setup_vm(t, t->name);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);
  t->test_pass = !HIT_TRAP(r);
  destroy_vm(t);
}

void simple_genter_handler(test_t *t) {
  regs_t r;
  setup_vm(t, t->name);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_CONFIG_EL1, 1);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_GXF_CONFIG_EL1, 1);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_PPERM_EL1, SPRR_BASE_VALUE);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);

  // We entered GL1 if:
  //   1. We jumped to the genter handler (in genter.s hardcoded to be at +0x100 from start)
  //   2. We executed 1 instruction of the genter handler (+4 bytes)
  //   3. We read 1 from currentg
  t->test_pass = (TEST_VIRT_HI + 0x104 == r.pc) && (1 == r.x0);
  destroy_vm(t);
}

void gxf_sp_handler(test_t *t) {
  regs_t r;
  uint64_t magic1 = 0x1111111111111111ull;
  uint64_t magic2 = 0x2222222222222222ull;
  setup_vm(t, t->name);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_CONFIG_EL1, 1);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_GXF_CONFIG_EL1, 1);
  hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_PPERM_EL1, SPRR_BASE_VALUE);
  hv_vcpu_set_reg(t->cpu0, HV_REG_X0, magic1);
  hv_vcpu_set_reg(t->cpu0, HV_REG_X1, magic2);
  run_until_infloop(t->cpu0, false);
  get_regs(t->cpu0, &r);

  t->test_pass = (r.sp_el1 == magic1) && (r.sp_gl1 == magic2) && ((r.cpsr & BIT(0)) != 0);
  destroy_vm(t);
}

void sprr_handler(test_t *t) {
#define NUM_PERMS 6
  regs_t r;
  bool perms[NUM_SPRR_POSSIBLE_SETTINGS][NUM_PERMS] = {0};
  const char *test_program[NUM_PERMS] = {
    "sprr_read_gl1",
    "sprr_write_gl1",
    "sprr_exec_gl1",
    "sprr_read_el1",
    "sprr_write_el1",
    "sprr_exec_el1",
  };

  for (size_t i = 0; i < NUM_SPRR_POSSIBLE_SETTINGS; i++) {
    for (size_t p = 0; p < NUM_PERMS; p++) {
      setup_vm(t, test_program[p]);
      hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_CONFIG_EL1, 1);
      hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_GXF_CONFIG_EL1, 1);
      hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_SPRR_PPERM_EL1, (i << 4) | SPRR_BASE_VALUE);
      hv_vcpu_set_sys_reg(t->cpu0, HV_SYS_REG_VBAR_GL1, VBAR_VIRT);
      hv_vcpu_set_reg(t->cpu0, HV_REG_X0, SPRR_VIRT_HI(1));
      run_until_infloop(t->cpu0, false);
      get_regs(t->cpu0, &r);

      perms[i][p] = !HIT_TRAP(r);
      destroy_vm(t);
    }
  }

  printf("\n");
  printf("    SPRR Permissions\n");
  printf("╭─────┬───────┬───────╮\n");
  printf("│     │  GL1  │  EL1  │\n");
  printf("│     │ R W X │ R W X │\n");
  printf("├─────┼───────┼───────┤\n");
  for (size_t i = 0; i < NUM_SPRR_POSSIBLE_SETTINGS; i++) {
    printf("│  %lX  │ %d %d %d │ %d %d %d │\n", i, perms[i][0], perms[i][1], perms[i][2], perms[i][3], perms[i][4], perms[i][5]);
  }
  printf("╰─────┴───────┴───────╯\n");

  t->test_pass = true;
#undef NUM_PERMS
}

int main() {
  int tests_run = 0, tests_pass = 0;
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    tests[i].handler(&tests[i]);
    tests_run++;
    if (tests[i].test_pass) tests_pass++;
    printf("[%s] %ld: %s (%s)\n", tests[i].test_pass ? "✅" : "❌", i, tests[i].name, tests[i].desc);
  }

  printf("%d of %d tests pass\n", tests_pass, tests_run);
}
