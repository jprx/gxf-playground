#include "defs.h"
.section .text
.global start

start:
  mov x0, #0x01
  msr sprr_config_el1, x0

  b .
