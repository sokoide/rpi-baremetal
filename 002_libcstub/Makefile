# Set CROSSROOT env var before building as below
# export CROSSROOT = /Volumes/AppleHDDCS/cross/rpi/arm-unknown-eabi

TARGET =	hoge.img
TARGETELF =	hoge.elf
ASRCS = startup.s \

CSRCS = hoge.c \
				lib/rpi-common.c \
				syscalls.c

LDS = hoge.lds

OBJS = $(ASRCS:.s=.o) $(CSRCS:.c=.o)

CROSS = arm-unknown-eabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy
CCFLAGS = -mfloat-abi=soft -mfpu=neon-vfpv4 -mlittle-endian -nostartfiles -ffreestanding -fno-builtin -c -O2 -std=c11 -D RPI2
LIBS = -L$(CROSSROOT)/lib/gcc/arm-unknown-eabi/4.8.4/ -L$(CROSSROOT)/arm-unknown-eabi/lib/
LDFLAGS = $(LIBS) -lc -lgcc


all: $(TARGET)


.SUFFIXES: .elf .img
.elf.img:
	$(OBJCOPY) -O binary $< $@

$(TARGETELF): $(OBJS)
	$(LD) -static -nostartup -T $(LDS) $^ -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CCFLAGS) -o $@ $<

.s.o:
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET) $(TARGETELF) $(OBJS) tags

