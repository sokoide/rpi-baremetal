@ startup
  .align

.global _start
_start:
// IRQ/FIQ disabled, Thumb disabled, initialize stack in IRQ mode
	ldr	r0, =0x000000d2
  #move ARM register to system register
	msr	cpsr, r0
	ldr	sp, =0x00008000

// IRQ/FIQ disabled, Thumb disabled, initialize stack in SVC mode
  ldr r0, =0x000000d3
  msr cpsr, r0
  ldr sp, =0x06400000

  b main
  b .

.global _hangup
_hangup:
	ldr	r0, =0x000000d3
	msr	cpsr, r0
	b .

_IRQ_iterrupt:
  // store multiple registers full descendent
  // same as stmdb (decrement before) and push w/ r13!
	stmfd	r13!, {r0-r12,lr}
	bl	IRQ_handler
  // load multiple registerd empty ascending
  // same as ldfia (increment after) and pop w/ r13!
	ldmfd	r13!, {r0-r12,lr}
	subs	pc,lr, #4

.global _enable_IRQ
_enable_IRQ:
  // move system to ARM register
	mrs	r0, cpsr
	bic r0, r0, #0x80
	msr cpsr_c,r0
	bx lr

.global _disable_IRQ
_disable_IRQ:
	mrs	r0, cpsr
	ldr	r1,	=0x80
	orr r0, r0, r1
	msr	cpsr_c, r0
	bx lr

.global _getmode
_getmode:
	mrs	r0, cpsr
	bx lr

.global  _initialize_vector_start
_initialize_vector_start:
	ldr	pc, _vec_Reset
	ldr	pc, _vec_Undef
	ldr	pc, _vec_SWI
	ldr	pc, _vec_PrefAbort
	ldr	pc, _vec_DataAbort
	ldr	pc, _vec_Reserved
	ldr	pc, _vec_IRQ
	ldr	pc, _vec_FIQ
_vec_Reset:		.word	_start
_vec_Undef:		.word	_hangup
_vec_SWI:		.word	_hangup
_vec_PrefAbort:	.word	_hangup
_vec_DataAbort:	.word	_hangup
_vec_Reserved:	.word	_hangup
_vec_IRQ:		.word	_IRQ_iterrupt
_vec_FIQ:		.word	_hangup
	.global  _initialize_vector_end
_initialize_vector_end:
// dummy instruction to keep initialize_vector_end label
	mov r0,r0

.global _init_vector_table
_init_vector_table:
  ldr r0, =0x0
  ldr r1, =_initialize_vector_start
  ldr r3, =_initialize_vector_end
  cmp r1, r3
  bxeq lr
  _init_vector_table_loop:
  ldr r2, [r1]
  str r2, [r0]
  add r1, #4
  add r0, #4
  cmp r1, r3
  bne _init_vector_table_loop
  bx lr

.global _get_stack_pointer
_get_stack_pointer:
  str sp, [sp]
  ldr r0, [sp]
  bx lr

.global _write_memory
_write_memory:
  bx lr

.global _wfi
_wfi:
  wfi
	bx lr

.global _context_switch
_context_switch:
  // same as stmfd/stmdb !r13, {...}
  push {r0-r12,r14}
  str sp, [r0]
  ldr sp, [r1]
  // same as ldmfd/ldmia !r13, {...}
  pop {r0-r12}
  pop {pc} // pc points to the previous lr

