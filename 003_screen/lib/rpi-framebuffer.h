#ifndef _RPI_FRAMEBUFFER_
#define _RPI_FRAMEBUFFER_

#include <stdint.h>
#include <stdbool.h>

#define kWidth 1920
#define kHeight 1080

struct FramebufferRequest {
  uint32_t width;
  uint32_t height;
  uint32_t vwidth;
  uint32_t vheight;
  uint32_t pitch;
  uint32_t depth;
  uint32_t x;
  uint32_t y;
  uint32_t pointer;
  uint32_t size;
} FBRequest __attribute__((aligned(4)));

bool FramebufferInitialize();

#endif
