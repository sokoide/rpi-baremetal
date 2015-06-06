# Raspberry Pi2 bare metal examples

### How to run image files
Copy build *.img into SD card in addition to bootcode.bin, config.txt and start.elf to run.
When RPi starts up, GPU loads bootloader from ROM which loads bootcode.bin which reads config.txt. Start.elf loads the boot image file written in config.txt at 0x00008000 and runs it.

Update config.txt and change kernel=... to your boot image.
```
kernel=hoge.img
disable_overscan=1
framebuffer_swap=0
```

#### 001_led
##### About
* Blink OK LED example on Raspberry Pi2.
* Used header file for GPIO from http://www.valvers.com/open-software/raspberry-pi/step02-bare-metal-programming-in-c-pt2/
* Used init code from http://tatsu-zine.com/books/raspi-bm
This version is not using timer interrupt but using busy loop for intervals.

##### For Raspberry Pi 1
* For B+, Remove -D RPI2 and add -D RPIBPLUS into Makefile.
* For A/A+/B, Remove -D RPI2 from Makefile.

#### 002_libcstub
##### About
* Cstub and printf to memory char by char
* Howeber, cant debug it with cross gdb due to "Unhandled v6 thumb insn: 4601"

#### 003_screen
##### About
* Get framebuffer address by MailBox and draw random dots

#### 004_timer_interrupt
##### About
* Dirty sample which uses timer interrupt
* It redraw every 100ms using timer

#### 005_character
##### About
* Print characters using hankaku.bin bitmap font

#### 006_fps
##### About
* Print frames per second

