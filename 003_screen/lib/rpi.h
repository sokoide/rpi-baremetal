#ifndef _RPI_H_
#define _RPI_H_

#include <sys/types.h>
#include <stdint.h>

#include "rpi-common.h"
#include "rpi-gpio.h"
#include "rpi-mailbox.h"
#include "rpi-framebuffer.h"

#define kFastInterval 0x2FFFFFUL
#define kRegularInterval 0x6FFFFFUL
#define kSlowInterval 0xAFFFFFUL

void rpi_init(void);

// defined in startup.s
extern caddr_t _get_stack_pointer(void);
extern void _write_memory(char c);

#endif
