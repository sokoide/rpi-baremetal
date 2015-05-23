#ifndef _RPI_MAILBOX_H_
#define _RPI_MAILBOX_H_

#include <stdint.h>
#include "rpi.h"

#define MAIL_BASE 0xB880
#define MAIL_READ 0x0
#define MAIL_CONFIG 0x1C
#define MAIL_STATUS 0x18
#define MAIL_WRITE 0x20

#define MAIL_POWER 0x0
#define MAIL_FB 0x1
#define MAIL_VUART 0x2
#define MAIL_VCHIQ 0x3
#define MAIL_LEDS 0x4
#define MAIL_BUTTONS 0x5
#define MAIL_TOUCH 0x6
#define MAIL_COUNT 0x7
#define MAIL_TAGS 0x8

// Mailbox Status Register: Mailbox Empty (There is nothing to read from the
// Mailbox)
#define MAIL_EMPTY 0x40000000

// Mailbox Status Register: Mailbox Full  (There is no space to write into the
// Mailbox)
#define MAIL_FULL 0x80000000

uint32_t MailboxRead();
void MailboxWrite(uint32_t data);

static uint32_t *mailBoxStatusRegister =
    (uint32_t *)(IO_BASE + MAIL_BASE + MAIL_READ + MAIL_STATUS);

#endif
