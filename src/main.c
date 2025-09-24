#include <efi.h>
#include <utils.h>
// #include <terminal.h>
// #include <textRenderer.h>
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
        // UINTN boxWidth = 30;
        //  UINT32 color[2] = {0x000000FF, 0x00129047};
        //  UINTN ind = 0;

        EFI_INPUT_KEY key;
        InitializeTerminal(framebuffer, 1000, 600, width, height);
        DrawBorder(framebuffer, 0xFFFFFFFF);
        // for (UINTN y = 0; y < height; y++)
        // {
        //         for (UINTN x = 0; x < width; x++)
        //         {

        //                 terminal_border_draw(framebuffer, x, y, width, height, 10);
        //         }
        // }
        while (1)
        {
                UpdateTerminal(framebuffer, SystemTable->ConIn);
                // for (volatile UINTN d = 0; d < 300000000; d++)
                //         ;
                // ind = (ind + 1) % 2;
        }

        CHAR16 buf[128];
        CHAR16 num[32];

        // Build "Resolution: <w>x<h>\r\n"
        buf[0] = L'\0';
        concat(buf, L"Resolution: ");

        utoa(gop->Mode->Info->HorizontalResolution, num);
        concat(buf, num);
        concat(buf, L"x");
        utoa(gop->Mode->Info->VerticalResolution, num);
        concat(buf, num);
        concat(buf, L"\r\n");

        // Print it
        SystemTable->ConOut->OutputString(SystemTable->ConOut, buf);

        // Wait for key
        // EFI_INPUT_KEY key;
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Press any key to exit...\r\n");
        BS->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &width);
        SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

        return EFI_SUCCESS;
}
