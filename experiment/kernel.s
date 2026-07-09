.section .text
.global start

#define GENTER .word 0x00201420
#define GEXIT  .word 0x00201400

#define SPRR_CONFIG_EL1  S3_6_c15_c1_0
#define GXF_CONFIG_EL1   S3_6_c15_c1_2

start:
  mov x0, #0x01
  msr SPRR_CONFIG_EL1, x0

  b .
