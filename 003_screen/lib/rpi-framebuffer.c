#include "rpi.h"
#include "rpi-framebuffer.h"
#include "rpi-mailbox.h"

const unsigned int FramebufferMailboxChannel = 1;

bool FramebufferInitialize() {
  fbRequest.size = sizeof(struct FramebufferRequest);
  fbRequest.bufferRequestResponseCode = 0;

  fbRequest.tag_setPd = 0x00048003;  // tag ID
  fbRequest.size_setPd = 8;          // value buffer size
  fbRequest.rr_setPd = 8;            // req/res indicator
  fbRequest.width_setPd = kWidth;
  fbRequest.height_setPd = kHeight;

  fbRequest.tag_setVb = 0x00048004;
  fbRequest.size_setVb = 8;
  fbRequest.rr_setVb = 8;
  fbRequest.width_setVb = kWidth;
  fbRequest.height_setVb = kHeight;

  fbRequest.tag_setDepth = 0x00048005;
  fbRequest.size_setDepth = 4;
  fbRequest.rr_setDepth = 4;
  fbRequest.bitsPerPixel = 8;

  fbRequest.tag_setVo = 0x00048009;
  fbRequest.size_setVo = 8;
  fbRequest.rr_setVo = 8;
  fbRequest.frameBufferOffsetX = 0;
  fbRequest.frameBufferOffsetY = 0;

  fbRequest.tag_setPlt = 0x0004800B;
  fbRequest.size_setPlt = 0x28;
  fbRequest.rr_setPlt = 0x28;
  fbRequest.firstPltOffset = 0;  // 0-255
  fbRequest.pltLength = 8;       // 1-256
  // AABBGGRR
  fbRequest.color1 = 0x00000000;
  fbRequest.color2 = 0xFF0000FF;
  fbRequest.color3 = 0xFF00FF00;
  fbRequest.color4 = 0xFF0000FF;
  fbRequest.color5 = 0xFFFFFF00;
  fbRequest.color6 = 0xFFFF00FF;
  fbRequest.color7 = 0xFFFFFF00;
  fbRequest.color8 = 0xFFFFFFFF;

  fbRequest.tag_allocB = 0x00040001;
  fbRequest.size_allocB = 8;
  fbRequest.rr_allocB = 8;
  fbRequest.fbBaseAddress = 0;
  fbRequest.fbSize = 0;

  fbRequest.tag_end = 0;

  while (true) {
    MailboxWrite(((uint32_t)(&fbRequest) + MAIL_TAGS));
    if (0 != fbRequest.fbBaseAddress) {
      ledBlink(3, kSlowInterval);
      break;
    }
  }

  return true;
}
