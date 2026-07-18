#include <Hypervisor/Hypervisor.h>
#include <arm64/hv/hv_kern_types.h>
#include "hv_private.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/syslimits.h>
#include <assert.h>
#include <string.h>
#include "hvf_utils.h"
#include "regs.h"
#include "aarch64.h"
#include "test.h"

struct mmap_t {
  void   *buf;
  size_t  len;
};

void check(const char *fn_name, hv_return_t rv) {
  if (HV_SUCCESS != rv) {
    perror(fn_name);
    exit(1);
  }
}

hv_return_t enable_impdef_extensions(hv_vcpu_t cpu) {
  hv_return_t rv;
  uint64_t ctrl_val;

  rv = _hv_vcpu_get_control_field(cpu, HV_CTRL_HCR_EL2, &ctrl_val);
  check("_hv_vcpu_get_control_field HCR_EL2", rv);

  rv = _hv_vcpu_set_control_field(cpu, HV_CTRL_HCR_EL2, ctrl_val & ~HCR_EL2_TIDCP);
  check("_hv_vcpu_set_control_field HCR_EL2", rv);

  rv = _hv_vcpu_set_control_field(cpu, HV_CTRL_HACR_EL2, HACR_EL2_PERM_ALL);
  check("_hv_vcpu_set_control_field HACR_EL2", rv);

  return HV_SUCCESS;
}

void run_until_infloop(hv_vcpu_t c, bool dump_traces) {
  regs_t r;
  uint64_t last_pc = -1;

  while(1) {
    get_regs(c, &r);
    step_cpu(c);
    if (r.pc == last_pc) break;
    if (dump_traces) dump_regs(&r);

    last_pc = r.pc;
  }
}

void step_cpu(hv_vcpu_t c) {
  uint64_t cpsr;
  hv_vcpu_get_reg(c, HV_REG_CPSR, &cpsr);
  hv_vcpu_set_reg(c, HV_REG_CPSR, cpsr | CPSR_ENABLE_SS);
  hv_vcpu_run(c);
}

void make_cpu(test_t *t) {
  hv_return_t rv;
  hv_vcpu_t cpu0;
  hv_vcpu_config_t cpu0_cfg;

  cpu0_cfg = hv_vcpu_config_create();

  rv = hv_vcpu_create(&cpu0, &t->cpu0_exit, cpu0_cfg);
  check("hv_vcpu_create", rv);

  rv = hv_vcpu_set_trap_debug_exceptions(cpu0, true);
  check("set debug trap", rv);

  rv = hv_vcpu_set_reg(cpu0, HV_REG_PC, t->needs_mmu ? TEST_VIRT_HI : TEST_PHYS);
  check("set pc", rv);

  rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_VBAR_EL1, t->needs_mmu ? VBAR_VIRT : VBAR_PHYS);
  check("set vbar", rv);

  rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_MDSCR_EL1, MDSCR_SS | MDSCR_KDE | MDSCR_MDE);
  check("set mdscr", rv);

  rv = hv_vcpu_set_reg(cpu0, HV_REG_CPSR, CPSR_ENABLE_SS | CPSR_MODE_EL1 | CPSR_IGNORE_INTS);
  check("set initial cpsr", rv);

  if (t->needs_mmu) {
    rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_TTBR0_EL1, L1_PTABLE_LO);
    check("set ttbr0", rv);

    rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_TTBR1_EL1, L1_PTABLE_HI);
    check("set ttbr1", rv);

    rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_TCR_EL1, TCR_V);
    check("set tcr", rv);

    rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_SCTLR_EL1, SCTLR_V);
    check("set sctlr", rv);
  }

  rv = enable_impdef_extensions(cpu0);
  check("enable_impdef_extensions", rv);

  os_release(cpu0_cfg);
  t->cpu0 = cpu0;
}

struct mmap_t load_kernel(const char *test_name) {
  int fd;
  void *buf;
  struct stat s;
  char path[PATH_MAX];
  snprintf(path, sizeof(path), TEST_DIR "/%s.bin", test_name);

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "couldn't open %s\n", path);
    perror("open");
    exit(1);
  }

  if (0 != fstat(fd, &s)) {
    perror("fstat");
    exit(1);
  }

  buf = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

  if (MAP_FAILED == buf) {
    perror("mmap");
    exit(1);
  }

  close(fd);

  return (struct mmap_t){
    .buf = buf,
    .len = s.st_size,
  };
}

void *alloc_page() {
  void *pt;

  pt = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (MAP_FAILED == pt) {
    perror("alloc_ptable");
    exit(1);
  }

  memset(pt, 0, PAGE_SIZE);
  return pt;
}

void free_page(void *t) {
  if (0 != munmap(t, PAGE_SIZE)) {
    perror("munmap page table");
    exit(1);
  }
}

void map_ptables() {
  hv_return_t rv;
  pte_t *l1_hi, *l1_lo;
  pte_t *l2_hi, *l2_lo, *l2_vbar;

  assert(2048  == NUM_PT_ENTRIES); // Confirm 16K pages
  assert(0 != (BIT(47) & TEST_VIRT_HI));
  assert(0 != (BIT(47) & VBAR_VIRT));
  assert(0 == (BIT(47) & TEST_VIRT_LO));
  assert(BIT(14) == PAGE_SIZE);
  assert(BIT(25) == HUGE_PAGE_SIZE);
  assert(0x03  == PTE_TABLE_DESC);
  assert(0x401 == PTE_BLOCK_ENTRY);
  assert(L1_PTABLE_IDX(VBAR_VIRT) != L1_PTABLE_IDX(TEST_VIRT_HI));
  assert(L2_PTABLE_IDX(TEST_VIRT_HI) != L2_PTABLE_IDX(SPRR_VIRT_HI(0)));
  assert(L2_PTABLE_IDX(TEST_VIRT_LO) != L2_PTABLE_IDX(SPRR_VIRT_LO(0)));
  assert(NUM_SPRR_REGIONS == NUM_SPRR_POSSIBLE_SETTINGS);

  l1_hi   = alloc_page();
  l2_hi   = alloc_page();
  l1_lo   = alloc_page();
  l2_lo   = alloc_page();
  l2_vbar = alloc_page();

  l1_hi[L1_PTABLE_IDX(TEST_VIRT_HI)] = L2_PTABLE_HI | PTE_TABLE_DESC;
  l1_lo[L1_PTABLE_IDX(TEST_VIRT_LO)] = L2_PTABLE_LO | PTE_TABLE_DESC;

  l2_hi[L2_PTABLE_IDX(TEST_VIRT_HI)] = TEST_PHYS | PTE_BLOCK_ENTRY;
  l2_lo[L2_PTABLE_IDX(TEST_VIRT_LO)] = TEST_PHYS | PTE_BLOCK_ENTRY;
  for (size_t i = 0; i < NUM_SPRR_REGIONS; i++) {
    l2_hi[L2_PTABLE_IDX(SPRR_VIRT_HI(i))] = TEST_PHYS | PTE_BLOCK_ENTRY | SPRR_IDX_TO_PTE(i);
    l2_lo[L2_PTABLE_IDX(SPRR_VIRT_LO(i))] = TEST_PHYS | PTE_BLOCK_ENTRY | SPRR_IDX_TO_PTE(i);
  }

  l1_hi[L1_PTABLE_IDX(VBAR_VIRT)] = L2_PTABLE_VBAR | PTE_TABLE_DESC;
  l2_vbar[L2_PTABLE_IDX(VBAR_VIRT)] = VBAR_PHYS | PTE_BLOCK_ENTRY;

  rv = hv_vm_map(l1_hi, L1_PTABLE_HI, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE);
  check("hv_vm_map l1_hi page table", rv);

  rv = hv_vm_map(l1_lo, L1_PTABLE_LO, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE);
  check("hv_vm_map l1_lo page table", rv);

  rv = hv_vm_map(l2_hi, L2_PTABLE_HI, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE);
  check("hv_vm_map l2_hi page table", rv);

  rv = hv_vm_map(l2_lo, L2_PTABLE_LO, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE);
  check("hv_vm_map l2_lo page table", rv);

  rv = hv_vm_map(l2_vbar, L2_PTABLE_VBAR, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE);
  check("hv_vm_map l2_vbar page table", rv);

  free_page(l1_hi);
  free_page(l2_hi);
  free_page(l1_lo);
  free_page(l2_lo);
  free_page(l2_vbar);
}

void map_vector_table() {
  hv_return_t rv;
  uint32_t *vector_table = alloc_page();

  for (size_t i = 0; i < PAGE_SIZE / sizeof(vector_table[0]); i++) {
    vector_table[i] = INSN_INFLOOP;
  }

  rv = hv_vm_map(vector_table, VBAR_PHYS, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC);
  check("map vector table", rv);

  free_page(vector_table);
}

void unmap_ptables() {
  hv_return_t rv;

  rv = hv_vm_unmap(L1_PTABLE_HI, PAGE_SIZE);
  check("hv_vm_unmap l1_hi ptable", rv);

  rv = hv_vm_unmap(L2_PTABLE_HI, PAGE_SIZE);
  check("hv_vm_unmap l2_hi ptable", rv);

  rv = hv_vm_unmap(L1_PTABLE_LO, PAGE_SIZE);
  check("hv_vm_unmap l1_lo ptable", rv);

  rv = hv_vm_unmap(L2_PTABLE_LO, PAGE_SIZE);
  check("hv_vm_unmap l2_lo ptable", rv);

  rv = hv_vm_unmap(L2_PTABLE_VBAR, PAGE_SIZE);
  check("hv_vm_unmap l2_vbar ptable", rv);
}

void map_kernel(const char *path) {
  hv_return_t rv;
  struct mmap_t kernel_f;

  kernel_f = load_kernel(path);
  if (!kernel_f.buf) {
    fprintf(stderr, "Unable to open kernel\n");
    exit(1);
  }

  assert(PAGE_SIZE == kernel_f.len);

  rv = hv_vm_map(kernel_f.buf, TEST_PHYS, PAGE_SIZE, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC);
  check("hv_vm_map", rv);

  if (0 != munmap(kernel_f.buf, kernel_f.len)) {
    perror("munmap kernel");
    exit(1);
  }
}

void setup_vm(test_t *t, const char *kernel_name) {
  hv_return_t rv;
  hv_vm_config_t cfg;

  cfg = hv_vm_config_create();
  rv = hv_vm_config_set_el2_enabled(cfg, false);
  check("set el2", rv);

  rv = _hv_vm_config_set_isa(cfg, ISA_PRIVATE);
  check("_hv_vm_config_set_isa", rv);

  rv = hv_vm_create(cfg);
  check("hv_vm_create", rv);

  map_kernel(kernel_name);
  map_vector_table();
  if (t->needs_mmu) map_ptables();

  os_release(cfg);
  make_cpu(t);
}

void destroy_vm(test_t *t) {
  hv_return_t rv;
  if (t->needs_mmu) unmap_ptables();

  rv = hv_vcpu_destroy(t->cpu0);
  check("hv_vcpu_destroy", rv);

  rv = hv_vm_unmap(TEST_PHYS, PAGE_SIZE);
  check("hv_vm_unmap kernel", rv);

  rv = hv_vm_unmap(VBAR_PHYS, PAGE_SIZE);
  check("hv_vm_unmap vbar", rv);

  rv = hv_vm_destroy();
  check("hv_vm_destroy", rv);
}
