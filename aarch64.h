#pragma once
#include <stdint.h>

#define BIT(n) (( 1ull << ((n)) ))

#define ONE_KB                 BIT(10)
#define ONE_MB                 BIT(20)
#define ONE_GB                 BIT(30)

#define CPSR_ENABLE_SS         BIT(21)
#define CPSR_MODE_EL1          BIT(0) | BIT(2)
#define CPSR_IGNORE_INTS       BIT(6) | BIT(7) | BIT(8)

#define MDSCR_SS               BIT(0)
#define MDSCR_MDE              BIT(15)
#define MDSCR_KDE              BIT(13)

#define HCR_EL2_TIDCP          BIT(20)
#define HACR_EL2_PERM_ALL      ((uint64_t)(-1ull))

// XNU sets TCR to 0x10800226511A511
// IPS = 40TB
// T0SZ, T1SZ = 17 (47 bit VAs in each half)
// TG0, TG1 = 16K pages (recall these have different encodings)
// Lower half:  0x0000000000000000 -> 0x00007FFFFFFFFFFF
// Higher half: 0xFFFF800000000000 -> 0xFFFFFFFFFFFFFFFF
#define TCR_IPS                (( 0b010ull   <<  32 ))
#define TCR_T0SZ               (( 0b10001ull <<  0  ))
#define TCR_T1SZ               (( 0b10001ull <<  16 ))
#define TCR_TG0                (( 0b10ull    <<  14 ))
#define TCR_TG1                (( 0b01ull    <<  30 ))
#define TCR_V                  (( TCR_IPS | TCR_T0SZ | TCR_T1SZ | TCR_TG0 | TCR_TG1 ))

#define SCTLR_M                BIT(0)
#define SCTLR_V                (( SCTLR_M ))

#define PAGE_SIZE_KB           16
#define PAGE_SIZE              PAGE_SIZE_KB * ONE_KB

#define HUGE_PAGE_SIZE_MB      32
#define HUGE_PAGE_SIZE         HUGE_PAGE_SIZE_MB * ONE_MB

#define L1_PTABLE_IDX(i)       (( ( ((i)) >> 36 ) & (BIT(11)-1) ))
#define L2_PTABLE_IDX(i)       (( ( ((i)) >> 25 ) & (BIT(11)-1) ))

#define VECTOR_TABLE_LEN       0x800
#define INSN_INFLOOP           0x14000000
#define INSN_NOP               0xD503201F

#define NUM_PT_ENTRIES         ((PAGE_SIZE) / (sizeof(uint64_t)))

#define PTE_INVALID            0
#define PTE_VALID              BIT(0)
#define PTE_POINTS_TO_TABLE    BIT(1)
#define PTE_POINTS_TO_BLOCK    0
#define PTE_AF                 BIT(10)
#define PTE_PRIV_RW            0

#define PTE_TABLE_DESC         (( PTE_POINTS_TO_TABLE | PTE_VALID ))
#define PTE_BLOCK_ENTRY        (( PTE_AF | PTE_POINTS_TO_BLOCK | PTE_PRIV_RW | PTE_VALID ))

#define PTE_BIT_AP0            6
#define PTE_BIT_AP1            7
#define PTE_BIT_PXN            53
#define PTE_BIT_UXN            54

// PXN = shift by 4  (1x) <- LSB
// UXN = shift by 8  (2x)
// AP0 = shift by 16 (4x)
// AP1 = shift by 32 (8x) <- MSB
#define SPRR_IDX_TO_PTE(i) \
  ((uint64_t)( 0 != (((i)) & BIT(0)) ) << PTE_BIT_PXN) | \
  ((uint64_t)( 0 != (((i)) & BIT(1)) ) << PTE_BIT_UXN) | \
  ((uint64_t)( 0 != (((i)) & BIT(2)) ) << PTE_BIT_AP0) | \
  ((uint64_t)( 0 != (((i)) & BIT(3)) ) << PTE_BIT_AP1)

// A value for sprr_pperm_el1 that gives sprr index 0000 R-X for both EL1 and GL1
// Found by guess and check
#define SPRR_BASE_VALUE 0x0000000000000005ull

typedef uint64_t pte_t;
