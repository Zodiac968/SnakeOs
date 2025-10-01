.PHONY: all

SRC := $(wildcard *.c)
OBJS := $(SRC:.c=.o)

CC = gcc \
	-Wl,--subsystem,10 \
	-e efi_main

CFLAGS = \
	-std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-mno-red-zone \
	-ffreestanding \
	-nostdlib \
	-I../include

all : libcommon.a

libcommon.a : $(OBJS)
	ar rcs $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c $^
