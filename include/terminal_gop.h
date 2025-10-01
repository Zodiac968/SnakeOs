#ifndef TERMINALGOP_H
#define TERMINALGOP_H

#include <efi.h>
#include <textRenderer.h>
#include <utils.h>
#include <snake.h>

#define MAX_LINES 50
#define MAX_COLS 300

extern CHAR16 terminalBuffer[MAX_LINES][MAX_COLS];
extern UINTN inputLine[MAX_LINES];
extern UINTN winWidth, winHeight;
extern UINTN fb_width, fb_height;
extern UINTN rows, cols;
extern UINTN cursor_x, cursor_y;
extern BOOLEAN runningSnake;

VOID InitializeTerminal(UINT32 *framebuffer, UINTN terminal_width, UINTN terminal_height, UINTN width, UINTN height);
VOID draw_line(UINT32 *framebuffer, UINTN grid_x, UINTN grid_y, const CHAR16 *str, UINT32 color);
UINTN UpdateTerminal(UINT32 *framebuffer, EFI_HANDLE ImageHandle, EFI_BOOT_SERVICES *BS, EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci);
VOID DrawBorder(UINT32 *framebuffer, UINT32 color);

#endif