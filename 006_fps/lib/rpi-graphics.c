#include "rpi.h"
#include "rpi-graphics.h"
#include "rpi-mailbox.h"
#include <string.h>

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
  fbRequest.color4 = 0xFFFF0000;
  fbRequest.color5 = 0xFFFFFF00;
  fbRequest.color6 = 0xFFFF00FF;
  fbRequest.color7 = 0xFF00FFFF;
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
      break;
    }
  }

  return true;
}

void FillRect(int x, int y, int width, int height, char color) {
  // left ... draw pixel by pixel
  int x1 = x;
  int x2 = ((x + 31) / 32) * 32;
  if (x1 != x2) {
    char *p8 = (char *)fbRequest.fbBaseAddress;
    p8 += y * kWidth + x1;
    for (int yy = y; yy < y + height; yy++) {
      for (int xx = x1; xx < x2; xx++) {
        *p8++ = color;
      }
      p8 += kWidth - (x2 - x1);
    }
  }

  // center draw 32 pixels at once
  int x3 = ((x + width) / 32) * 32;
  if (x2 != x3) {
    uint32_t *p = (uint32_t *)fbRequest.fbBaseAddress;
    p += (y * kWidth + x2) / 4;
    for (int yy = y; yy < y + height; yy++) {
      for (int xx = x2; xx < x3; xx += 4 * 8) {
        int32_t c = color << 24 | color << 16 | color << 8 | color;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
        *p++ = c;
      }
      p += (kWidth - (x3 - x2)) / 4;
    }
  }

  // right ... draw pixel by pixel
  int x4 = x + width;
  if (x3 != x4) {
    char *p8 = (char *)fbRequest.fbBaseAddress;
    p8 += y * kWidth + x3;
    for (int yy = y; yy < y + height; yy++) {
      for (int xx = x3; xx < x4; xx++) {
        *p8++ = color;
      }
      p8 += kWidth - (x4 - x3);
    }
  }
}

void DrawRect(int x, int y, int width, int height, char color) {
  // TODO
}

void PrintStr(int x, int y, char *str, char color) {
  size_t len = strlen(str);
  for (int i = 0; i < len; i++) {
    char c = str[i];
    PrintChar(x + i * 8, y, c, color);
  }
}

void PrintChar(int x, int y, char c, char color) {
  char *hankaku = (char *)&_binary_hankaku_bin_start;
  char *base = (char *)fbRequest.fbBaseAddress;
  char *p;
  char d;

  for (int i = 0; i < 16; i++) {
    p = base + (y + i) * kWidth + x;
    d = hankaku[c * 16 + i];
    if ((d & 0x80) != 0) {
      p[0] = color;
    }
    if ((d & 0x40) != 0) {
      p[1] = color;
    }
    if ((d & 0x20) != 0) {
      p[2] = color;
    }
    if ((d & 0x10) != 0) {
      p[3] = color;
    }
    if ((d & 0x08) != 0) {
      p[4] = color;
    }
    if ((d & 0x04) != 0) {
      p[5] = color;
    }
    if ((d & 0x02) != 0) {
      p[6] = color;
    }
    if ((d & 0x01) != 0) {
      p[7] = color;
    }
  }
}
