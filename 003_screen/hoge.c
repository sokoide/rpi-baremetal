#include "lib/rpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define kWidth 1920
#define kHeight 1080

int main(int argc, char const* argv[]) {
  rpiInit();
  ledBlink(3, kFastInterval);

  // init framebuffer
  if (!FramebufferInitialize()) {
    ledBlink(10, kSlowInterval);
    while (true)
      ;
  }

  ledBlink(3, kFastInterval);

  uint16_t color = 0;
  uint16_t* p = (uint16_t*)FBRequest.pointer;
  for (int x = 0; x < kWidth; x++) {
    for (int y = 0; y < kHeight; y++) {
      *p++ = color++;
    }
  }

  ledBlink(3, kFastInterval);

  while (true) {
    ledBlink(3, kFastInterval);
  }
  return 0;
}
