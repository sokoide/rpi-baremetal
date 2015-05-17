#include "lib/rpi.h"
#define kFastInterval 200000
#define kRegularInterval 500000
#define kSlowInterval 1000000

volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;
volatile unsigned int tim;

void wait(unsigned int interval);

void wait(unsigned int interval) {
  for (tim = 0; tim < interval; tim++)
    ;
}

void led_on() {
  /* Set the LED GPIO pin high ( Turn OK LED off for original Pi, and on
     for plus models )*/
  gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
}

void led_off() {
  /* Set the LED GPIO pin low ( Turn OK LED on for original Pi, and off
     for plus models )*/
  gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);
}

int main(int argc, char const* argv[]) {
  rpiInit();

  gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

  unsigned int intervals[] = {kFastInterval, kRegularInterval, kSlowInterval,
                              kRegularInterval};
  while (1) {
    for (int i = 0; i < sizeof(intervals) / sizeof(intervals[0]); i++) {
      unsigned int interval = intervals[i];
      {
        for (int j = 0; j < 3; j++) {
          wait(interval);
          led_on();
          wait(interval);
          led_off();
        }
      }
    }
  }

  return 0;
}
