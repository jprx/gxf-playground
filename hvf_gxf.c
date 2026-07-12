#include <Hypervisor/Hypervisor.h>
#include <Hypervisor/hv_vcpu.h>
#include <arm64/hv/hv_kern_types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "regs.h"

#define BIT(n) (( 1ull << ((n)) ))

#define ISA_PRIVATE            4

#define CPSR_ENABLE_SS         BIT(21)
#define CPSR_MODE_EL1          BIT(0) | BIT(2)
#define CPSR_IGNORE_INTS       BIT(6) | BIT(7) | BIT(8)

#define MDSCR_SS               BIT(0)
#define MDSCR_MDE              BIT(15)
#define MDSCR_KDE              BIT(13)

#define HCR_EL2_TIDCP          BIT(20)
#define HACR_EL2_PERM_ALL      ((uint64_t)(-1ull))

#define KBASE 0x40000000

extern uint64_t     _hv_vm_config_get_isa(hv_vm_config_t);
extern hv_return_t  _hv_vm_config_set_isa(hv_vm_config_t, uint64_t);
extern hv_return_t  _hv_vcpu_set_control_field(hv_vcpu_t, uint64_t, uint64_t);
extern hv_return_t  _hv_vcpu_get_control_field(hv_vcpu_t, uint64_t, uint64_t*);
extern void        *_hv_vcpu_get_context(hv_vcpu_t);
extern void         _hv_capability(uint32_t idx, int64_t *);

enum {
  HV_CTRL_HCR_EL2          =  0,
  HV_CTRL_TRAP_DEBUG_MODE  =  2, // 0x100 = trap debug exceptions, 0x200 = trap debug registers
  HV_CTRL_HACR_EL2         =  9,
} hv_control_field_t;

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

void *cpu_handler(void *_unused) {
  regs_t r;
  hv_return_t rv;
  hv_vcpu_t cpu0;
  hv_vcpu_config_t cpu0_cfg;
  hv_vcpu_exit_t *cpu0_exit;

  cpu0_cfg = hv_vcpu_config_create();

  rv = hv_vcpu_create(&cpu0, &cpu0_exit, cpu0_cfg);
  check("hv_vcpu_create", rv);

  rv = hv_vcpu_set_trap_debug_exceptions(cpu0, true);
  check("set debug trap", rv);

  r.pc = KBASE;
  rv = hv_vcpu_set_reg(cpu0, HV_REG_PC, r.pc);
  check("set pc", rv);

  rv = hv_vcpu_set_sys_reg(cpu0, HV_SYS_REG_MDSCR_EL1, MDSCR_SS | MDSCR_KDE | MDSCR_MDE);
  check("set mdscr", rv);

  rv = hv_vcpu_set_reg(cpu0, HV_REG_CPSR, CPSR_ENABLE_SS | CPSR_MODE_EL1 | CPSR_IGNORE_INTS);
  check("set initial cpsr", rv);

  rv = enable_impdef_extensions(cpu0);
  check("enable_impdef_extensions", rv);

  for (size_t i = 0; i < 3; i++) {
    hv_vcpu_run(cpu0);

    get_regs(cpu0, &r);

    r.cpsr |= CPSR_ENABLE_SS;
    hv_vcpu_set_reg(cpu0, HV_REG_CPSR, r.cpsr);

    dump_regs(&r);
    printf("debug syndrome: 0x%llX\n", cpu0_exit->exception.syndrome);
  }

  if (r.pc == KBASE + 8) {
    fprintf(stderr, "successfully accessed sprr_config_el1\n");
    exit(1);
  } else {
    printf("failed to access sprr_config_el1\n");
    exit(0);
  }

  hv_vcpu_destroy(cpu0);
  os_release(cpu0_cfg);
  return NULL;
}

struct mmap_t load_kernel(const char *path) {
  int fd;
  void *buf;
  struct stat s;

  fd = open(path, O_RDONLY);
  if (fd < 0) {
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

  return (struct mmap_t){
    .buf = buf,
    .len = s.st_size,
  };
}

int main(int argc, char **argv) {
  pthread_t cpu_thread;
  hv_return_t rv;
  hv_vm_config_t cfg;
  struct mmap_t kernel_f;

  kernel_f = load_kernel("experiment/kernel.bin");
  if (!kernel_f.buf) {
    fprintf(stderr, "Unable to open kernel\n");
    exit(1);
  }

  cfg = hv_vm_config_create();
  rv = hv_vm_config_set_el2_enabled(cfg, false);
  check("set el2", rv);

  rv = _hv_vm_config_set_isa(cfg, ISA_PRIVATE);
  check("_hv_vm_config_set_isa", rv);

  rv = hv_vm_create(cfg);
  check("hv_vm_create", rv);

  rv = hv_vm_map(kernel_f.buf, KBASE, kernel_f.len, HV_MEMORY_READ | HV_MEMORY_WRITE | HV_MEMORY_EXEC);
  check("hv_vm_map", rv);

  pthread_create(&cpu_thread, NULL, cpu_handler, NULL);
  pthread_join(cpu_thread, NULL);

  rv = hv_vm_unmap(KBASE, kernel_f.len);
  check("hv_vm_unmap", rv);

  os_release(cfg);
  munmap(kernel_f.buf, kernel_f.len);
  return 0;
}
