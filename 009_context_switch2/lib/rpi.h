#ifndef _RPI_H_
#define _RPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef RPI2
#define IO_BASE 0x3F000000UL
#else
#define IO_BASE 0x20000000UL
#endif

#include "rpi-peripherals.h"

#define kFastInterval 0x3FFFFUL
#define kRegularInterval 0x5FFFFUL
#define kSlowInterval 0x7FFFFUL

// defined in startup.s *****
extern void _enable_IRQ(void);
extern void _disable_IRQ(void);
extern uint32_t _getmode(void);
extern void _init_vector_table(void);
extern caddr_t _get_stack_pointer(void);
extern void _write_memory(char c);
extern void _wfi();
extern void _context_switch(int **from_sp, int **to_sp);

// defined in hankaku.o *****
extern char *_binary_hankaku_bin_start;
extern char *_binary_hankaku_bin_end;
extern uint32_t *_binary_hankaku_bin_size;

// common *****
void rpiInit(void);
void clearBss(void);

// graphics *****
#define kWidth 640
#define kHeight 480

// mailbox buffer should be aligned with 16 bytes
typedef struct {
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
} FramebufferRequest __attribute__((aligned(16)));
extern FramebufferRequest fbRequest;

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

// fifo *****
typedef struct {
  unsigned char *buf;
  int p, q, size, free, flags;
} FIFO8;
void InitFifo8(FIFO8 *fifo, int size, unsigned char *buf);
int PutFifo8(FIFO8 *fifo, unsigned char data);
int GetFifo8(FIFO8 *fifo);
int StatusFifo8(FIFO8 *fifo);

// timer *****
#define MAX_TIMERS 512
typedef struct _TIMER {
  int id;
  unsigned int timeout;
  unsigned char data;
  bool used;
  struct _TIMER *next;
} TIMER;

void InitTimerInterrupt(FIFO8 *fifo);
void InitTimerCtl(FIFO8 *fifo);
int CreateTimer();
void InitTimer(int id);
void DeleteTimer(int id);
int SetTimer(int id, unsigned int timeout, unsigned char data);

void InsertTimer(int id);
bool RemoveTimer(int id);
TIMER *TimerAt(int index);

typedef struct {
  unsigned int counter, next;
  unsigned int length;  // number of used timers
  FIFO8 *fifo;
  TIMER timer[MAX_TIMERS];
  TIMER *head;
} TIMERCTL;

extern TIMERCTL timerctl;

// thread ***
#define MAX_THREADS 1024
void InitThread();
void CreateThread(void *thread_entry);
unsigned int ContextSwitch(int sp);

typedef enum {
  THREAD_NONE,
  THREAD_CREATED,
  THREAD_RUNNING,
  THREAD_WAITING
} THREAD_STATE;

typedef struct {
  THREAD_STATE state;
  int *stack;
  unsigned int stackSize;
} THREAD;

typedef struct {
  unsigned int currentId;
  unsigned int length;
  THREAD thread[MAX_THREADS];
} THREADCTL;

extern THREADCTL threadctl;

#ifdef __cplusplus
}
#endif
#endif
