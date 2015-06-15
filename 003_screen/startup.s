@ startup
  .global _start
  .global _get_stack_pointer
  .global _write_memory
  .align

_start:
  ldr r0, =0x000000d3
  msr cpsr, r0
  ldr sp, =0x06400000

  #main
  b main
  b .

_get_stack_pointer:
  mov r0, r13
  mov pc, lr

_write_memory:
  bx  lr

