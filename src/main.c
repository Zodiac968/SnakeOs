#include <efi.h>

// ==== Function prototypes ====
VOID utoa(UINTN value, CHAR16 *buffer);
VOID concat(CHAR16 *dest, CONST CHAR16 *src);

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

        // Get framebuffer
        UINT32 *framebuffer = (UINT32 *)gop->Mode->FrameBufferBase;
        UINTN width = gop->Mode->Info->HorizontalResolution;
        UINTN height = gop->Mode->Info->VerticalResolution;
        UINTN boxWidth = 30;
        UINT32 color[2] = {0x000000FF, 0x00129047};
        UINTN ind = 0;

        while (1)
        {
                // Fill screen with green
                for (UINTN y = 0; y < height; y++)
                {
                        for (UINTN x = 0; x < width; x++)
                        {
                                if (x >= width / 2 - boxWidth && x <= width / 2 + boxWidth && y >= height / 2 - boxWidth && y <= height / 2 + boxWidth)
                                {
                                        framebuffer[y * width + x] = color[ind]; // Green (BGRA)
                                }
                                else
                                {
                                        framebuffer[y * width + x] = 0x00000000; // Black (BGRA)
                                }
                        }
                }

                for (volatile UINTN d = 0; d < 300000000; d++)
                        ;
                ind = (ind + 1) % 2;
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
        EFI_INPUT_KEY key;
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Press any key to exit...\r\n");
        BS->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, &width);
        SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

        return EFI_SUCCESS;
}

// Convert unsigned integer to CHAR16 string (decimal)
VOID utoa(UINTN value, CHAR16 *buffer)
{
        CHAR16 temp[32];
        INTN i = 0, j = 0;

        // Handle zero
        if (value == 0)
        {
                buffer[0] = L'0';
                buffer[1] = L'\0';
                return;
        }

        // Extract digits in reverse
        while (value > 0)
        {
                temp[i++] = (CHAR16)(L'0' + (value % 10));
                value /= 10;
        }

        // Reverse them into buffer
        while (i > 0)
        {
                buffer[j++] = temp[--i];
        }
        buffer[j] = L'\0';
}

VOID concat(CHAR16 *dest, CONST CHAR16 *src)
{
        while (*dest)
                dest++;
        while (*src)
                *dest++ = *src++;
        *dest = L'\0';
}