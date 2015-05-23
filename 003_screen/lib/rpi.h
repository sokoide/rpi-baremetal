#ifndef _RPI_H_
#define _RPI_H_

#include <sys/types.h>
#include <stdint.h>

#include "rpi-common.h"
#include "rpi-gpio.h"
#include "rpi-mailbox.h"
#include "rpi-framebuffer.h"

#define kFastInterval 0x3FFFFUL
#define kRegularInterval 0x5FFFFUL
#define kSlowInterval 0x7FFFFUL

void rpi_init(void);

// defined in startup.s
extern caddr_t _get_stack_pointer(void);
extern void _write_memory(char c);

#endif
