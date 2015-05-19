#ifndef RPILIB_H
#define RPILIB_H

#include "rpi-common.h"
#include "rpi-gpio.h"
#include <sys/types.h>

void rpi_init(void);

// defined in startup.s
extern caddr_t _get_stack_pointer(void);
extern void _write_memory(char c);

#endif
