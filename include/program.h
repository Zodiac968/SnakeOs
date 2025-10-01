#ifndef PROGRAM_H
#define PROGRAM_H

#include <efi.h>
#include <utils.h>

typedef struct
{
    EFI_DEVICE_PATH_PROTOCOL Header;
    CHAR16 PathName[1]; // variable length
} FILEPATH_DEVICE_PATH;
typedef struct
{
    EFI_DEVICE_PATH_PROTOCOL Header;
} END_DEVICE_PATH;

static UINTN DevicePathSize(EFI_DEVICE_PATH_PROTOCOL *DevicePath)
{
    EFI_DEVICE_PATH_PROTOCOL *Node = DevicePath;
    UINTN Size = 0;

    while (!(Node->Type == 0x7F && Node->SubType == 0xFF))
    {
        Size += (Node->Length[0] | (Node->Length[1] << 8));
        Node = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)Node + (Node->Length[0] | (Node->Length[1] << 8)));
    }

    // include END node (4 bytes)
    Size += 4;
    return Size;
}

EFI_DEVICE_PATH_PROTOCOL *MakeFileDevicePath(EFI_BOOT_SERVICES *BS, CHAR16 *FilePath, EFI_STATUS *status);
EFI_DEVICE_PATH_PROTOCOL *AppendDevicePathManual(
    EFI_BOOT_SERVICES *BS,
    EFI_DEVICE_PATH_PROTOCOL *BasePath,
    EFI_DEVICE_PATH_PROTOCOL *AppendPath,
    EFI_STATUS *Status);
EFI_STATUS RunEfiApplication(EFI_HANDLE *ImageHandle, EFI_BOOT_SERVICES *BS, CHAR16 *filePath);

#endif