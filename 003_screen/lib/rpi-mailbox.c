#include "rpi-mailbox.h"
#include "rpi-gpio.h"

uint32_t MailboxRead() {
  // Loop until found for channel
  uint32_t status = 0;
  for (;;) {
    status = *mailBoxStatusRegister;
    while ((status & MAIL_EMPTY) != 0) {
      /* while ((MBReadRegister(MailboxStatusRegister) & MailEmptyFlag) != 0) {
       */
      // Wait until there is data to read
      ledBlink(2, kFastInterval);
    }

    uint32_t *mailBoxReadTarget = (uint32_t *)(IO_BASE + MAIL_BASE + MAIL_READ);
    // TODO
    return *mailBoxReadTarget;
  }
}

void MailboxWrite(uint32_t data) {
  uint32_t status = 0;
  status = *mailBoxStatusRegister;

  while ((status & MAIL_FULL) != 0) {
    // Wait until there is space to write
    ledBlink(1, kFastInterval);
    status = *mailBoxStatusRegister;
  }

  uint32_t *mailBoxWriteTarget =
      (uint32_t *)(IO_BASE + MAIL_BASE + MAIL_WRITE + MAIL_TAGS);
  *mailBoxWriteTarget = data;
}
