.POSIX:
.PHONY: all clean

SOURCE := $(wildcard src/*.c)
OBJS := $(SOURCE:.c=.o)
TARGET = snake.EFI

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
	-I../include \
	-I../libs

LDFLAGS = \
	-L../libs -lcommon

all: $(TARGET)
	copy $(TARGET) ..\applications

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@ 

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	cd ./src && del main.o
	del $(TARGET)