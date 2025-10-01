#include <terminal_gop.h>
#include <program.h>

#define MAX_LINES 50
#define MAX_COLS 300

CHAR16 terminalBuffer[MAX_LINES][MAX_COLS];
UINTN inputLine[MAX_LINES];
UINTN winWidth, winHeight;
UINTN fb_width, fb_height;
UINTN rows, cols;
UINTN cursor_x, cursor_y;
EFI_INPUT_KEY key;
CHAR16 blankLine[MAX_COLS];
BOOLEAN runningSnake;

VOID InitializeTerminal(UINT32 *framebuffer, UINTN terminal_width, UINTN terminal_height, UINTN width, UINTN height)
{
    fb_width = width;
    fb_height = height;
    winWidth = terminal_width;
    winHeight = terminal_height;
    rows = winHeight / 16;
    cols = winWidth / 8;
    for (UINTN i = 0; i <= cols; i++)
        blankLine[i] = L' ';
    blankLine[cols + 1] = L'\0';
    runningSnake = FALSE;
    concat(terminalBuffer[0], L"cmd>");
    draw_line(framebuffer, 0, 0, terminalBuffer[0], 0xFFFFFFFF);
    cursor_x = 4;
    cursor_y = 0;
    inputLine[0] = 1;
}

VOID DrawBorder(UINT32 *framebuffer, UINT32 color)
{
    UINTN offset_x = fb_width / 2 - winWidth / 2;
    UINTN offset_y = fb_height / 2 - winHeight / 2;
    for (UINTN i = 0; i < cols + 3; i++)
    {
        draw_char(framebuffer, fb_width, i * 8 + offset_x - 8, offset_y - 16, &vgafont[205 * 16], color);
        draw_char(framebuffer, fb_width, i * 8 + offset_x - 8, winHeight + offset_y + 16, &vgafont[205 * 16], color);
    }
    for (UINTN i = 0; i < rows + 2; i++)
    {
        draw_char(framebuffer, fb_width, offset_x - 16, i * 16 + offset_y, &vgafont[186 * 16], color);
        draw_char(framebuffer, fb_width, winWidth + offset_x + 16, i * 16 + offset_y, &vgafont[186 * 16], color);
    }
    draw_char(framebuffer, fb_width, offset_x - 16, offset_y - 16, &vgafont[201 * 16], color);                        // top left
    draw_char(framebuffer, fb_width, offset_x - 16, winHeight + offset_y + 16, &vgafont[200 * 16], color);            // bottom left
    draw_char(framebuffer, fb_width, winWidth + offset_x + 16, offset_y - 16, &vgafont[187 * 16], color);             // top right
    draw_char(framebuffer, fb_width, winWidth + offset_x + 16, winHeight + offset_y + 16, &vgafont[188 * 16], color); // bottom right
}

VOID draw_line(UINT32 *framebuffer, UINTN grid_x, UINTN grid_y, const CHAR16 *str, UINT32 color)
{
    UINTN offset_x = fb_width / 2 - winWidth / 2;
    UINTN offset_y = fb_height / 2 - winHeight / 2;
    draw_string(framebuffer, fb_width, grid_x * 8 + offset_x, grid_y * 16 + offset_y, str, color);
}

VOID redraw_terminal(UINT32 *framebuffer)
{
    DrawBorder(framebuffer, 0xFFFFFFFF);
    for (UINTN i = 0; i < rows; i++)
    {
        draw_line(framebuffer, 0, i, blankLine, 0xFFFFFFFF);
        if (i < cursor_y + 1)
            draw_line(framebuffer, 0, i, terminalBuffer[i], 0xFFFFFFFF);
    }
}

UINTN UpdateTerminal(UINT32 *framebuffer, EFI_HANDLE ImageHandle, EFI_BOOT_SERVICES *BS, EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci)
{
    UINT32 white = 0xFFFFFFFF;
    if (ci->ReadKeyStroke(ci, &key) == EFI_SUCCESS)
    {
        if (key.UnicodeChar == L'\r')
        {
            CHAR16 msg[50];
            if (streq(terminalBuffer[cursor_y] + 4, L"exit"))
            {
                return 0;
            }
            else if (streq(terminalBuffer[cursor_y] + 4, L"clear"))
            {
                while (cursor_y > 0)
                {
                    clrstr(terminalBuffer[cursor_y]);
                    inputLine[cursor_y] = 0;
                    draw_line(framebuffer, 0, cursor_y, blankLine, white);
                    cursor_y--;
                }
                clrstr(terminalBuffer[0]);
                concat(terminalBuffer[0], L"cmd>");
                draw_line(framebuffer, 0, 0, blankLine, white);
                draw_line(framebuffer, 0, 0, terminalBuffer[0], white);
                cursor_x = 4;
                cursor_y = 0;
                inputLine[0] = 1;
                goto over;
            }
            else
            {
                EFI_STATUS status;
                CHAR16 filePath[100] = L"\\EFI\\";
                concat(filePath, terminalBuffer[cursor_y] + 4);
                UINTN len = string_length(filePath);
                if (!streq(filePath + len - 3, L".efi") || !streq(filePath + len - 3, L".EFI"))
                {
                    concat(filePath, L".efi");
                }
                status = RunEfiApplication(ImageHandle, BS, filePath);
                if (EFI_ERROR(status))
                    concat(msg, L"Command not found");
                else
                {
                    redraw_terminal(framebuffer);
                    concat(msg, L" ");
                }
            }
            cursor_y++;
            cursor_x = 0;
            concat(terminalBuffer[cursor_y], msg);
            draw_line(framebuffer, 0, cursor_y, terminalBuffer[cursor_y], white);
            cursor_y++;
            clrstr(msg);
            concat(terminalBuffer[cursor_y], L"cmd>");
            inputLine[cursor_y] = 1;
            draw_line(framebuffer, 0, cursor_y, terminalBuffer[cursor_y], white);
            cursor_x = 4;
        }
        else if (key.UnicodeChar == L'\b')
        {
            if (cursor_x == 0)
            {
                cursor_x = cols;
                cursor_y--;
            }
            if (inputLine[cursor_y] == 0 || cursor_x > 4)
            {
                backspace(terminalBuffer[cursor_y]);
                draw_line(framebuffer, --cursor_x, cursor_y, L" ", white);
            }
        }
        else
        {
            CHAR16 ch[4];
            ch[0] = key.UnicodeChar;
            ch[1] = L'\0';
            concat(terminalBuffer[cursor_y], ch);
            draw_line(framebuffer, cursor_x, cursor_y, terminalBuffer[cursor_y] + cursor_x, white);
            cursor_x++;
            if (cursor_x == cols + 1)
            {
                cursor_x = 0;
                cursor_y++;
            }
        }
    }
over:
    return 1;
}
