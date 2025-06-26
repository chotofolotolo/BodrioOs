/* link.ld */
OUTPUT_FORMAT("binary")
ENTRY(main)
SECTIONS {
  /*
   * Cambiamos la dirección base a 0x10000.
   * Esta es una dirección de memoria segura, por encima de los
   * primeros 64KB que usa la BIOS.
   */
  . = 0x10000;

  .text : { *(.text*) }
  .data : { *(.data*) }
  .bss  : { *(.bss*)  }
}