#pragma once
#include <Hypervisor/Hypervisor.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include "aarch64.h"

#define TEST_DIR "tests"

#define TEST_PHYS                   0x0000000040000000ull
#define VBAR_PHYS                   0x0000000FF0000000ull

#define VBAR_VIRT                   0xFFFFFFFF00000000ull
#define TEST_VIRT_HI                0xFFFF800040000000ull
#define TEST_VIRT_LO                0x0000000040000000ull

// SPRR uses 4 PTE bits -> 2^4 = 16 possible SPRR settings
// Each possible configuration gets 1 virtual region
#define NUM_SPRR_BITS               4
#define NUM_SPRR_POSSIBLE_SETTINGS  ((1ull << NUM_SPRR_BITS))
#define NUM_SPRR_REGIONS            NUM_SPRR_POSSIBLE_SETTINGS

#define SPRR_VIRT_HI(i)             (( TEST_VIRT_HI + ( HUGE_PAGE_SIZE * (1 + ((i))) ) ))
#define SPRR_VIRT_LO(i)             (( TEST_VIRT_LO + ( HUGE_PAGE_SIZE * (1 + ((i))) ) ))

#define PAGETABLE_BASE              0x20000000
#define PAGETABLE(n)                (( PAGETABLE_BASE + ( PAGE_SIZE * ((n)) ) ))

#define L1_PTABLE_HI                PAGETABLE(0)
#define L2_PTABLE_HI                PAGETABLE(1)
#define L1_PTABLE_LO                PAGETABLE(2)
#define L2_PTABLE_LO                PAGETABLE(3)
#define L2_PTABLE_VBAR              PAGETABLE(4)

struct test_t;
typedef void (*test_handler_t)(struct test_t*);

typedef struct test_t {
  char           *name;
  char           *desc;
  test_handler_t  handler;
  bool            needs_mmu;
  bool            test_pass;
  hv_vcpu_t       cpu0;
  hv_vcpu_exit_t *cpu0_exit;
} test_t;
