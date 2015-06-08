CSRCS = teststub.c \
				lib/rpi-common.c \
				lib/rpi-mailbox.c \
				lib/rpi-graphics.c \
				lib/rpi-fifo.c \
				lib/rpi-timer.c \
				# lib/rpi-syscall.c \

CCSRCS = ../googletest/src/gtest-all.cc \
					 testmain.cc \

TARGET = hoge_test
OBJS = $(CCSRCS:.cc=.o) $(CSRCS:.c=.o) \

CC=clang-3.6 -c -std=c11 -D RPI2
# CC=gcc-4.8 -c -std=c11 -D RPI2
CXX=clang++-3.6 -std=c++11 -stdlib=libc++ -c
LD=clang++-3.6
CCFLAGS=
CXXFLAGS=-nostdinc++ -g -I/usr/local/opt/llvm36/lib/llvm-3.6/include/c++/v1 -Wall -I../googletest/include -I../googletest
LDFLAGS=-L/usr/local/opt/llvm36/lib/llvm-3.6/lib
LIBS =

all: $(TARGET)

testrun: $(TARGET)
	chmod +x $(TARGET)
	$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.c.o:
	$(CC) $(CCFLAGS) -o $@ $<

.s.o:
	$(CC) $(CCFLAGS) -o $@ $<

.cc.o:
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJS) tags

