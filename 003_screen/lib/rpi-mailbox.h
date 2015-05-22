#ifndef _RPI_MAILBOX_H_
#define _RPI_MAILBOX_H_

#include <stdint.h>
#include "rpi.h"

const static uint32_t* MailboxReadRegister =
    (uint32_t*)(IO_BASE + 0x0000B880UL);
const static uint32_t* MailboxWriteRegister =
    (uint32_t*)(IO_BASE + 0x0000B8A0UL);
const static uint32_t* MailboxStatusRegister =
    (uint32_t*)(IO_BASE + 0x0000B898UL);
const static uint32_t MailFullFlag = 0x80000000UL;
const static uint32_t MailEmptyFlag = 0x40000000UL;

uint32_t MBReadRegister(const uint32_t* address);
void MBWriteRegister(uint32_t* address, uint32_t data);
uint32_t MailboxRead(uint8_t channel);
void MailboxWrite(uint8_t channel, uint32_t data);

#endif
