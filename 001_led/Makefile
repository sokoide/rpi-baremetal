# how to debug with arm simulator with gdb
# 1. arm-unknown-eabi-gdb
# 2. target sim
# 3. load hoge.elf
# 4. file hoge.elf
# 5. start
# 6. disass, si, info r etc

TARGET =	hoge.img
TARGETELF =	hoge.elf
ASRCS = startup.s \

CSRCS = hoge.c \
				rpi_lib/rpi_init.c \
				rpi_lib/bss/clearbss.c \

LDS = hoge.lds

OBJS = $(ASRCS:.s=.o) $(CSRCS:.c=.o)

CROSS = arm-unknown-eabi-
CC = $(CROSS)gcc
LD = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy
# CCFLAGS = -mfloat-abi=hard -mlittle-endian -nostdinc -fno-builtin -c -O2
CCFLAGS = -mfloat-abi=soft -mlittle-endian -nostdinc -fno-builtin -c -O2 -std=c11 -D RPI2
LDFLAGS = -static -nostdlib -T $(LDS)


all: $(TARGET)


.SUFFIXES: .elf .img
.elf.img:
	$(OBJCOPY) -O binary $< $@

$(TARGETELF): $(OBJS)
	$(LD) $(LDFLAGS) -T $(LDS) $^ `$(CC) -print-libgcc-file-name` -o $@

.c.o:
	$(CC) $(CCFLAGS) -o $@ $<

.s.o:
	$(CC) $(CCFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET) $(TARGETELF) $(OBJS) tags

