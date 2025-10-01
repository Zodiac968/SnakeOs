#include <efi.h>
#include <terminal_gop.h>
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
        SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
        // Get framebuffer(
        UINT32 *framebuffer = (UINT32 *)gop->Mode->FrameBufferBase;
        UINTN width = gop->Mode->Info->HorizontalResolution;
        UINTN height = gop->Mode->Info->VerticalResolution;
        EFI_INPUT_KEY key;
        InitializeTerminal(framebuffer, 1000, 600, width, height);

        DrawBorder(framebuffer, 0xFFFFFFFF);
        while (1)
        {
                UINTN status = UpdateTerminal(framebuffer, ImageHandle, BS, SystemTable->ConIn);
                if (!status)
                        break;
        }
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Press any key to exit...\r\n");
        BS->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &width);
        SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

        return EFI_SUCCESS;
}
