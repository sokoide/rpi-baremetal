#ifndef RPI_COMMON_H
#define RPI_COMMON_H

#ifdef RPI2
#define IO_BASE 0x3F000000UL
#else
#define IO_BASE 0x20000000UL
#endif

void rpiInit(void);
void clearBss(void);

#endif
