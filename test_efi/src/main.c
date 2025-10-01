#include <efi.h>
#include <utils.h>

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
    BS->WaitForEvent(1, &SystemTable->ConIn->WaitForKey, NULL);
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);

    return EFI_SUCCESS;
}
