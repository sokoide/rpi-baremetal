#ifndef _RPI_H_
#define _RPI_H_

#include <sys/types.h>
#include <stdint.h>

#include "rpi-common.h"
#include "rpi-peripherals.h"
#include "rpi-mailbox.h"
#include "rpi-graphics.h"

#define kFastInterval 0x3FFFFUL
#define kRegularInterval 0x5FFFFUL
#define kSlowInterval 0x7FFFFUL

// defined in startup.s
extern void _enable_IRQ(void);
extern void _disable_IRQ(void);
extern uint32_t _getmode(void);
extern void _init_vector_table(void);
extern caddr_t _get_stack_pointer(void);
extern void _write_memory(char c);

// defined in hankaku.o
extern char* _binary_hankaku_bin_start;
extern char* _binary_hankaku_bin_end;
extern uint32_t* _binary_hankaku_bin_size;
#endif
