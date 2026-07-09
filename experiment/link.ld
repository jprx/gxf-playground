ENTRY(start);

SECTIONS {
  . = 0x40000000;
  .text : {
    *(.text);
    . = ALIGN(16K);
  }
}
