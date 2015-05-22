#include "rpi-gpio.h"

volatile unsigned int* gpio = (unsigned int*)GPIO_BASE;
volatile unsigned int tim;

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

void ledBlink(unsigned int count, unsigned int interval) {
  for (unsigned int i = 0; i < count; i++) {
    led_on();
    wait(interval);
    led_off();
    wait(interval);
  }
}

void wait(unsigned int interval) {
  for (tim = 0; tim < interval; tim++)
    ;
}
