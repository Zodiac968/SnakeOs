#include <terminal.h>

CHAR16 terminalBuffer[MAX_LINES][MAX_COL];
UINTN inputLine[MAX_LINES];
UINTN text_cols, text_rows;
UINTN cursor_row, cursor_col;
CHAR16 blankLine[MAX_COL + 1];
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *co;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci;
EFI_INPUT_KEY key;

VOID InitializeTerminal(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *conout, EFI_SIMPLE_TEXT_INPUT_PROTOCOL *conin)
{
    co = conout;
    ci = conin;
    co->QueryMode(co, co->Mode->Mode, &text_cols, &text_rows);
    co->ClearScreen(co);
    for (UINTN i = 0; i < MAX_COL; i++)
        blankLine[i] = L' ';
    blankLine[MAX_COL] = L'\0';
    concat(terminalBuffer[0], L"cmd>");
    co->SetCursorPosition(co, 0, 2);
    co->OutputString(co, terminalBuffer[0]);
    cursor_col = 4;
    cursor_row = 0;
    inputLine[0] = 1;
}

VOID UpdateTerminal()
{
    if (ci->ReadKeyStroke(ci, &key) == EFI_SUCCESS)
    {
        if (key.UnicodeChar == L'\r')
        {
            CHAR16 msg[50];
            if (streq(terminalBuffer[cursor_row] + 4, L"snake"))
            {
                concat(msg, L"Snake game is executed.");
            }
            else if (streq(terminalBuffer[cursor_row] + 4, L"clear"))
            {
                while (cursor_row > 0)
                {
                    clrstr(terminalBuffer[cursor_row]);
                    inputLine[cursor_row] = 0;
                    co->SetCursorPosition(co, 0, cursor_row + 2);
                    co->OutputString(co, blankLine);
                    cursor_row--;
                }
                clrstr(terminalBuffer[0]);
                concat(terminalBuffer[0], L"cmd>");
                co->SetCursorPosition(co, 0, 2);
                co->OutputString(co, blankLine);
                co->SetCursorPosition(co, 0, 2);
                co->OutputString(co, terminalBuffer[0]);
                cursor_col = 4;
                cursor_row = 0;
                inputLine[0] = 1;
                goto over;
            }
            else
            {
                concat(msg, L"Command not found");
            }
            cursor_row++;
            cursor_col = 0;
            concat(terminalBuffer[cursor_row], msg);
            co->SetCursorPosition(co, 0, cursor_row + 2);
            co->OutputString(co, terminalBuffer[cursor_row]);
            cursor_row++;
            clrstr(msg);
            concat(terminalBuffer[cursor_row], L"cmd>");
            inputLine[cursor_row] = 1;
            co->SetCursorPosition(co, 0, cursor_row + 2);
            co->OutputString(co, terminalBuffer[cursor_row]);
            cursor_col = 4;
        }
        else if (key.UnicodeChar == L'\b')
        {
            if (cursor_col == 0)
            {
                cursor_col = MAX_COL - 1;
                cursor_row--;
            }
            if (inputLine[cursor_row] == 0 || cursor_col > 4)
            {
                backspace(terminalBuffer[cursor_row]);
                co->SetCursorPosition(co, --cursor_col, cursor_row + 2);
                co->OutputString(co, L" ");
            }
        }
        else
        {
            CHAR16 ch[4];
            ch[0] = key.UnicodeChar;
            ch[1] = L'\0';
            concat(terminalBuffer[cursor_row], ch);
            co->SetCursorPosition(co, cursor_col, cursor_row + 2);
            co->OutputString(co, terminalBuffer[cursor_row] + cursor_col);
            cursor_col++;
            if (cursor_col == MAX_COL - 1)
            {
                cursor_col = 0;
                cursor_row++;
            }
        }
    over:
    }
}

VOID terminal_border_draw(UINT32 *framebuffer, UINTN x, UINTN y, UINTN width, UINTN height, UINTN thickness)
{
    UINTN margin = 10;
    if ((x > margin && x <= margin + thickness) || (x >= width - margin - thickness && x < width - margin) || (y > margin && y <= margin + thickness) || (y >= height - margin - thickness && y < height - margin))
    {
        framebuffer[y * width + x] = 0xFFFFFFFF;
    }
}

VOID terminal_clear(UINT32 *framebuffer, UINTN x, UINTN y, UINTN width, UINTN height, UINTN thickness)
{
    UINTN margin = 10;
    if ((x > margin + thickness && x < margin - width - thickness && y > margin + thickness && y < height - margin - thickness))
    {
        framebuffer[y * width + x] = 0x00000000;
    }
}