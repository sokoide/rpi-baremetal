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
  FramebufferInitialize();

  uint32_t color = 0;
  while (true) {
    color = rand() % 8;
    uint32_t* p = (uint32_t*)fbRequest.fbBaseAddress;
    for (int y = 0; y < kHeight; y++) {
      for (int x = 0; x < kWidth; x += 4 * 8) {
        int32_t c = color << 24 | color << 16 | color << 8 | color;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        color = rand() % 8;
      }
    }
  }

  while (true) {
    ledBlink(3, kFastInterval);
  }
  return 0;
}
