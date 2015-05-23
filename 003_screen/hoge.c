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

  uint8_t color = 0;
  while (true) {
    color = rand() % 8;
    uint8_t* p = (uint8_t*)fbRequest.fbBaseAddress;
    for (int y = 0; y < kHeight; y++) {
      for (int x = 0; x < kWidth; x++) {
        *p++ = color;
        if (x % 8 == 0) {
          color = rand() % 8;
        }
      }
    }
    wait(kSlowInterval);
  }

  while (true) {
    ledBlink(3, kFastInterval);
  }
  return 0;
}
