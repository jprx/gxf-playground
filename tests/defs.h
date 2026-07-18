#pragma once

#define  genter .word 0x00201420
#define  gexit  .word 0x00201400

#define  sprr_config_el1          S3_6_c15_c1_0
#define  sprr_amrange_el1         S3_6_c15_c1_3
#define  sprr_pperm_el1           S3_6_c15_c1_6
#define  sprr_pperm_el2           S3_6_c15_c1_7
#define  sprr_uperm_el0           S3_6_c15_c1_5
#define  sprr_pmprr_el1           S3_6_c15_c3_1
#define  sprr_umprr_el1           S3_6_c15_c3_0
#define  sprr_pperm_sh01_el1      S3_6_c15_c4_2
#define  sprr_pperm_sh02_el1      S3_6_c15_c4_3
#define  sprr_pperm_sh03_el1      S3_6_c15_c4_4
#define  sprr_pperm_sh04_el1      S3_6_c15_c4_5
#define  sprr_pperm_sh05_el1      S3_6_c15_c4_6
#define  sprr_pperm_sh06_el1      S3_6_c15_c4_7
#define  sprr_pperm_sh07_el1      S3_6_c15_c5_0
#define  sprr_uperm_sh01_el1      S3_6_c15_c3_3
#define  sprr_uperm_sh02_el1      S3_6_c15_c3_4
#define  sprr_uperm_sh03_el1      S3_6_c15_c3_5
#define  sprr_uperm_sh04_el1      S3_6_c15_c3_6
#define  sprr_uperm_sh05_el1      S3_6_c15_c3_7
#define  sprr_uperm_sh06_el1      S3_6_c15_c4_0
#define  sprr_uperm_sh07_el1      S3_6_c15_c4_1

#define  gxf_config_el1           S3_6_c15_c1_2
#define  gxf_entry_el1            S3_6_c15_c8_1
#define  gxf_pabentry_el1         S3_6_c15_c8_2

#define  sp_gl1                   S3_6_c15_c10_0
#define  tpidr_gl1                S3_6_c15_c10_1
#define  aspsr_gl1                S3_6_c15_c10_4
#define  vbar_gl1                 S3_6_c15_c10_2
#define  far_gl1                  S3_6_c15_c10_7
#define  esr_gl1                  S3_6_c15_c10_5
#define  elr_gl1                  S3_6_c15_c10_6
#define  spsr_gl1                 S3_6_c15_c10_3
#define  pmcr1_gl1                S3_6_c15_c8_7
#define  afsr1_gl1                S3_6_c15_c0_1
#define  currentg                 S3_6_c15_c8_0
