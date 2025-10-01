#ifndef SNAKE_H
#define SNAKE_H

#include <efi.h>
#include <utils.h>
#include <textRenderer.h>

extern UINT64 seed;

extern UINT32 *framebuffer;
extern UINTN width;
extern UINTN height;

extern UINTN gameW;
extern UINTN gameH;
extern UINTN snakeCols;
extern UINTN snakeRows;

VOID toString(UINT32 n, CHAR16 *str);
UINT32 random();
VOID drawBox(UINTN x1, UINTN y1, UINTN x2, UINTN y2, UINT32 col);
VOID drawCell(UINTN x, UINTN y, UINT32 col, UINTN cellSize);
VOID snakeInitialise(UINT32 *fb, UINTN w, UINTN h);
BOOLEAN snakeUpdate(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci);
VOID snakeClear();

#endif