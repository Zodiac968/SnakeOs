#ifndef TERMINAL_H
#define TERMINAL_H

#include <efi.h>
#include <utils.h>

#define MAX_LINES 50
#define MAX_COL 100

extern CHAR16 terminalBuffer[MAX_LINES][MAX_COL];
extern UINTN inputLine[MAX_LINES];
extern UINTN text_cols, text_rows;
extern UINTN cursor_row, cursor_col;
extern CHAR16 blankLine[MAX_COL + 1];
extern EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *co;
extern EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci;
extern EFI_INPUT_KEY key;

void InitializeTerminal(EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *conout, EFI_SIMPLE_TEXT_INPUT_PROTOCOL *conin);
void UpdateTerminal();
void terminal_border_draw(UINT32 *framebuffer, UINTN x, UINTN y, UINTN width, UINTN height, UINTN thickness);
void terminal_clear(UINT32 *framebuffer, UINTN x, UINTN y, UINTN width, UINTN height, UINTN thickness);

#endif
