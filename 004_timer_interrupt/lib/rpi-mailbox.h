#ifndef _RPI_MAILBOX_H_
#define _RPI_MAILBOX_H_

#include <stdint.h>
#include "rpi.h"

uint32_t MailboxRead();
void MailboxWrite(uint32_t data);

static uint32_t *mailBoxStatusRegister =
    (uint32_t *)(IO_BASE + MAIL_BASE + MAIL_READ + MAIL_STATUS);

#endif
