#include "defs.h"
.section .text
.global start

start:
  add x0, x0, target - start
  ldr xzr, [x0]
  b .

.balign 0x100
target:
  ret
