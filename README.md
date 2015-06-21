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

### How to setup your cross compile environment
#### Build ARM cross compiler for OSX/Linux
* [OSX only] create a case sensitive partition
* clone the crosstool-ng repo and build it
```
mkdir -p /Volumes/AppleHDDCS/cross/src
cd /Volumes/AppleHDDCS/cross/src
git clone https://github.com/crosstool-ng/crosstool-ng
crosstool-ng
./bootstrap
./configure
make
make install
```
* download cross compiler config template
```
mkdir -p /Volumes/AppleHDDCS/cross/ct-ng_rpi
cd /Volumes/AppleHDDCS/cross/ct-ng_rpi
// download template
ct-ng arm-unknown-eabi
```
* configure ct-ng
```
ct-ng menuconfig
```
* change these
```
* path
  local tarballs directory -> /Volumes/AppleHDDCS/cross/ct-ng_rpi
  prerix directory -> /Volumes/AppleHDDCS/cross/rpi/{CT_TARGET}
* target options
  disable MMU
  arch -> ()
  emit assembly for CPU -> cortex-a7
  tune -> ()
  vfp -> neon-vfpv4
  fpu -> software (nofpu)
* c compiler
  gcc version -> 4.8.4 (4.9/5.0 fails to build)
  disable link libstdc++ statically
* debug facilities
  enable gcc and sim
* companion libraries
  isl -> 12.2 (14.0 fails)
```
* build it ... takes time. It took 12 mins on my macmini core i7 2.6GHz x 4 cores
```
ct-ng build
```

#### Set env var for my makefile
```
export PATH="$PATH:/Volumes/AppleHDDCS/cross/rpi/arm-unknown-eabi/bin"
export CROSSROOT = /Volumes/AppleHDDCS/cross/rpi/arm-unknown-eabi
```

### Examples
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

#### 007_wfi
##### About
* Use WFI to wait until interrupted
* Implemented timer which supports up to MAX_TIMER
* Added unittest runs on build machine in Test.mak

#### 008_context_switch
##### About
* Creates 3 threads including the one for main
* Each thread has its own stack
* When context switches per timer interrupt, it pushes the current registers for the thread, and save the stack pointer in the thread context
* main -> other thread is switched by timer interrupt, but task_a/task_b to other is not timer based. Those context switches will be fully automatic in the next example.

#### 009_context_switch2
##### About
* Implemented preemptive multi-tasking by using timer interrupt
* Bug: If you use timer which is <20ms, it may hide the current/other thread's PrintStr
* Couldn't use ldrex/strex/swp and it uses buggy critical secrtion (not actually mutually exclusive)

