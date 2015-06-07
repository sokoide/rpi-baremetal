#ifndef RPI_COMMON_H
#define RPI_COMMON_H

#include <stdint.h>
#include <stdbool.h>

#ifdef RPI2
#define IO_BASE 0x3F000000UL
#else
#define IO_BASE 0x20000000UL
#endif

// common *****
void rpiInit(void);
void clearBss(void);

// graphics *****
#define kWidth 640
#define kHeight 480

// mailbox buffer should be aligned with 16 bytes
struct FramebufferRequest {
  uint32_t size;
  uint32_t bufferRequestResponseCode;

  // Set_Physical_Display
  uint32_t tag_setPd;
  uint32_t size_setPd;
  uint32_t rr_setPd;
  uint32_t width_setPd;
  uint32_t height_setPd;

  // Set_Virtual_Buffer
  uint32_t tag_setVb;
  uint32_t size_setVb;
  uint32_t rr_setVb;
  uint32_t width_setVb;
  uint32_t height_setVb;

  // Set_Depth
  uint32_t tag_setDepth;
  uint32_t size_setDepth;
  uint32_t rr_setDepth;
  uint32_t bitsPerPixel;

  // Set_Virtual_Offset
  uint32_t tag_setVo;
  uint32_t size_setVo;
  uint32_t rr_setVo;
  uint32_t frameBufferOffsetX;
  uint32_t frameBufferOffsetY;

  // Set_Palette
  uint32_t tag_setPlt;
  uint32_t size_setPlt;
  uint32_t rr_setPlt;
  uint32_t firstPltOffset;
  uint32_t pltLength;
  uint32_t color1;
  uint32_t color2;
  uint32_t color3;
  uint32_t color4;
  uint32_t color5;
  uint32_t color6;
  uint32_t color7;
  uint32_t color8;

  // Allocate_Buffer
  uint32_t tag_allocB;
  uint32_t size_allocB;
  uint32_t rr_allocB;
  uint32_t fbBaseAddress;
  uint32_t fbSize;

  uint32_t tag_end;
} fbRequest __attribute__((aligned(16)));

bool FramebufferInitialize();

void FillRect(int x, int y, int width, int height, char color);
void DrawRect(int x, int y, int width, int height, char color);

void PrintStr(int x, int y, char *str, char color);
void PrintChar(int x, int y, char c, char color);

// mailbox *****
uint32_t MailboxRead();
void MailboxWrite(uint32_t data);

static uint32_t *mailBoxStatusRegister =
    (uint32_t *)(IO_BASE + MAIL_BASE + MAIL_READ + MAIL_STATUS);

#endif
