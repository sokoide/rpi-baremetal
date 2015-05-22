#include "rpi-mailbox.h"
#include "rpi-gpio.h"

uint32_t MBReadRegister(const uint32_t* address) { return *address; }

void MBWriteRegister(uint32_t* address, uint32_t data) { *address = data; }

uint32_t MailboxRead(uint8_t channel) {
  // Loop until found for channel
  for (;;) {
    while ((MBReadRegister(MailboxStatusRegister) & MailEmptyFlag) != 0) {
      // Wait until there is data to read
      ledBlink(2, kFastInterval);
    }

    uint32_t data = MBReadRegister(MailboxReadRegister);
    uint8_t readChannel = data & 0xF;
    data >>= 4;

    if (channel == readChannel) {
      return data;
    }
  }
}

void MailboxWrite(uint8_t channel, uint32_t data) {
  while ((MBReadRegister(MailboxStatusRegister) & MailFullFlag) != 0) {
    // Wait until there is space to write
    ledBlink(2, kRegularInterval);
  }

  MBWriteRegister((uint32_t*)MailboxWriteRegister, (data << 4) & channel);
}
