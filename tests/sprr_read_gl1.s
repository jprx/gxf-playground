#include "defs.h"
.section .text
.global start

start:
  add x0, x0, target - start
  adr x1, enter_gl1
  msr gxf_entry_el1, x1
  genter
  b .

.balign 0x100
enter_gl1:
  ldr xzr, [x0]
  b .

.balign 0x100
target:
  ret
