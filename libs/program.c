#include <program.h>

EFI_DEVICE_PATH_PROTOCOL *MakeFileDevicePath(EFI_BOOT_SERVICES *BS, CHAR16 *FilePath, EFI_STATUS *status)
{
    UINTN FilePathLen = string_length(FilePath);
    UINTN FileNodeSize = sizeof(EFI_DEVICE_PATH_PROTOCOL) + (FilePathLen + 1) * sizeof(CHAR16);
    UINTN EndNodeSize = sizeof(END_DEVICE_PATH);
    UINTN TotalSize = FileNodeSize + EndNodeSize;

    FILEPATH_DEVICE_PATH *Node = NULL;
    *status = BS->AllocatePool(EfiBootServicesData, TotalSize, (VOID **)&Node);
    if (EFI_ERROR(*status))
        return NULL;

    // Fill FILEPATH_DEVICE_PATH
    Node->Header.Type = 0x04;
    Node->Header.SubType = 0x04;
    Node->Header.Length[0] = (UINT8)(FileNodeSize & 0xFF);
    Node->Header.Length[1] = (UINT8)((FileNodeSize >> 8) & 0xFF);

    for (UINTN i = 0; i <= FilePathLen; i++)
        Node->PathName[i] = FilePath[i];

    // Fill END_DEVICE_PATH immediately after FILEPATH_DEVICE_PATH
    END_DEVICE_PATH *EndNode = (END_DEVICE_PATH *)((UINT8 *)Node + FileNodeSize);
    EndNode->Header.Type = 0x7F;
    EndNode->Header.SubType = 0xFF;
    EndNode->Header.Length[0] = 4;
    EndNode->Header.Length[1] = 0;

    return (EFI_DEVICE_PATH_PROTOCOL *)Node;
}

EFI_DEVICE_PATH_PROTOCOL *AppendDevicePathManual(
    EFI_BOOT_SERVICES *BS,
    EFI_DEVICE_PATH_PROTOCOL *BasePath,
    EFI_DEVICE_PATH_PROTOCOL *AppendPath,
    EFI_STATUS *Status)
{
    UINTN BaseSize = DevicePathSize(BasePath);
    UINTN AppendSize = DevicePathSize(AppendPath);
    UINTN TotalSize = BaseSize + AppendSize - 4; // don’t double-count END node

    EFI_DEVICE_PATH_PROTOCOL *NewPath = NULL;
    *Status = BS->AllocatePool(EfiBootServicesData, TotalSize, (VOID **)&NewPath);
    if (EFI_ERROR(*Status))
        return NULL;

    // Copy base path (including its END)
    UINT8 *Ptr = (UINT8 *)NewPath;
    for (UINTN i = 0; i < (BaseSize - 4); i++)
    {
        ((UINT8 *)NewPath)[i] = ((UINT8 *)BasePath)[i];
    }
    Ptr += BaseSize - 4;
    // Copy append path (including its END)
    for (UINTN j = 0; j < AppendSize; j++)
    {
        ((UINT8 *)NewPath)[BaseSize - 4 + j] = ((UINT8 *)AppendPath)[j];
    }

    return NewPath;
}

EFI_STATUS RunEfiApplication(EFI_HANDLE *ImageHandle, EFI_BOOT_SERVICES *BS, CHAR16 *filePath)
{
    EFI_STATUS status = EFI_SUCCESS;
    EFI_LOADED_IMAGE_PROTOCOL *lip = NULL;
    EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    status = BS->OpenProtocol(ImageHandle,
                              &lip_guid,
                              (VOID **)&lip,
                              ImageHandle,
                              NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status))
        return status;

    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    EFI_GUID dp_guid = EFI_DEVICE_PATH_PROTOCOL_GUID;
    status = BS->OpenProtocol(lip->DeviceHandle,
                              &dp_guid,
                              (VOID **)&DevicePath,
                              ImageHandle,
                              NULL,
                              EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status))
        return status;

    EFI_HANDLE NewImageHandle = NULL;
    EFI_DEVICE_PATH_PROTOCOL *FileNode = MakeFileDevicePath(BS, filePath, &status);
    if (EFI_ERROR(status))
        return status;
    EFI_DEVICE_PATH_PROTOCOL *FullPath = AppendDevicePathManual(BS, DevicePath, FileNode, &status);
    if (EFI_ERROR(status))
        return status;

    status = BS->LoadImage(
        FALSE,          // BootPolicy = FALSE, normal load
        ImageHandle,    // ParentImageHandle
        FullPath,       // Device path to the EFI file
        NULL,           // Source buffer (not needed here)
        0,              // Source size
        &NewImageHandle // Receives handle to the loaded image
    );
    if (!EFI_ERROR(status))
    {
        status = BS->StartImage(NewImageHandle, NULL, NULL);
    }
    if (FileNode)
        BS->FreePool(FileNode);
    if (FullPath)
        BS->FreePool(FullPath);
    return status;
}