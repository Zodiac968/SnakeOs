#include <efi.h>
#include <snake.h>
// ==== Function prototypes ====

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    (void)ImageHandle;
    EFI_STATUS Status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    // No InitializeLib, just use SystemTable directly
    EFI_BOOT_SERVICES *BS = SystemTable->BootServices;

    // Locate GOP
    Status = BS->LocateProtocol(&gopGuid, NULL, (void **)&gop);
    if (EFI_ERROR(Status))
    {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Unable to locate GOP\r\n");
        return Status;
    }
    // Get framebuffer(
    UINT32 *framebuffer = (UINT32 *)gop->Mode->FrameBufferBase;
    UINTN width = gop->Mode->Info->HorizontalResolution;
    UINTN height = gop->Mode->Info->VerticalResolution;
    EFI_INPUT_KEY key;
    snakeInitialise(framebuffer, width, height);
    BOOLEAN runningSnake = TRUE;
    while (1)
    {
        seed++;
        runningSnake = snakeUpdate(SystemTable->ConIn);
        for (volatile UINTN d = 0; d < 30000000; d++)
            ;
        if (!runningSnake)
        {
            snakeClear();
            break;
        }
    }

    return EFI_SUCCESS;
}
