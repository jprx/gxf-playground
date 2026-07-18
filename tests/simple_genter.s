#include "defs.h"
.section .text
.global start

start:
  adr x1, enter_gl1
  msr gxf_entry_el1, x1
  genter

.balign 0x100
enter_gl1:
  mrs x0, currentg
  b .
