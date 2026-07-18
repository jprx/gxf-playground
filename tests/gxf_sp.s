#include "defs.h"
.section .text
.global start

start:
  mov sp, x0
  adr x10, enter_gl1
  msr gxf_entry_el1, x10
  genter

.balign 0x100
enter_gl1:
  mov sp, x1
  b .
