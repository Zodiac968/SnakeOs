.POSIX:
.PHONY: all clean

SOURCE = src/main.c
TARGET = BOOTX64.EFI

ifeq ($(OS), Windows_NT)
QEMU = qemu.bat
DISK_FLAGS = --vhd
else
QEMU = ./qemu.sh
DISK_FLAGS =
endif

# x86_64-w64-mingw32- Uncomment for gcc, or move to OS check above, etc.
CC = gcc \
	-Wl,--subsystem,10 \
	-e efi_main

# Uncomment for clang, or move to OS check above, etc.
#CC = clang \
 	-target x86_64-unknown-windows \
 	-fuse-ld=lld-link \
 	-Wl,-subsystem:efi_application \
 	-Wl,-entry:efi_main

CFLAGS = \
	-std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-mno-red-zone \
	-ffreestanding \
	-nostdlib \
	-Iinclude

DISK_IMG_FOLDER = /UEFI-GPT-image-creator
DISK_IMG_PGM    = write_gpt.exe

all: $(DISK_IMG_FOLDER)/$(DISK_IMG_PGM) $(TARGET)
	cd .$(DISK_IMG_FOLDER) && $(QEMU)
	del $(TARGET)
	cd .$(DISK_IMG_FOLDER) && del test.hdd
	cd .$(DISK_IMG_FOLDER) && del test.vhd

$(DISK_IMG_FOLDER)/$(DISK_IMG_PGM):
	cd .$(DISK_IMG_FOLDER) && $(MAKE)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $<
	copy $(TARGET) UEFI-GPT-image-creator/$(TARGET)
	cd .$(DISK_IMG_FOLDER) && $(DISK_IMG_PGM) $(DISK_FLAGS)

clean:
	del $(TARGET)
	cd .$(DISK_IMG_FOLDER) && del test.hdd
	cd .$(DISK_IMG_FOLDER) && del test.vhd