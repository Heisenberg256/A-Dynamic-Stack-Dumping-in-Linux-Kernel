export PATH:=/opt/iot-devkit/1.7.2/sysroots/x86_64-pokysdk-linux/usr/bin:/opt/iot-devkit/1.7.2/sysroots/x86_64-pokysdk-linux/usr/bin/i586-poky-linux:$(PATH)

PWD:= $(shell pwd)
SROOT=/opt/iot-devkit/1.7.2/sysroots/i586-poky-linux
KDIR:=$(SROOT)/usr/src/kernel

LDLIBS = -L$(SROOT)/usr/lib
CCFLAGS = -I$(SROOT)/usr/include/libnl3
EXTRA_CFLAGS += -Wall
CC = i586-poky-linux-gcc

ARCH = x86
CROSS_COMPILE = i586-poky-linux-

APP = dump_stack_tester

obj-m:= dump_stack_tester.o

all:
	$(CC) -Wall -o $(APP) dump_stack_tester.c -lpthread $(CCFLAGS) -lnl-genl-3 -lnl-3
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f *.o $(APP)