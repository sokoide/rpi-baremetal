#include "rpi.h"
#include "rpi-framebuffer.h"
#include "rpi-mailbox.h"

const unsigned int FramebufferMailboxChannel = 1;

bool FramebufferInitialize() {
  FBRequest.width = kWidth;
  FBRequest.height = kHeight;
  FBRequest.vwidth = kWidth;
  FBRequest.vheight = kHeight;
  FBRequest.pitch = 0;
  FBRequest.depth = 16;
  FBRequest.x = 0;
  FBRequest.y = 0;
  FBRequest.pointer = 0;
  FBRequest.size = 0;

  MailboxWrite(1, (uint32_t)&FBRequest);
  uint32_t response = MailboxRead(1);

  if (response != 0) {
    // Error
    return false;
  }
  return true;
}
