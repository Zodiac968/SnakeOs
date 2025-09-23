#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <efi.h>
extern unsigned char vgafont[4096];
extern unsigned int vgafont_len;
void draw_char(UINT32 *framebuffer, UINTN fb_width, UINTN x, UINTN y, unsigned char *glyph, UINT32 color);
void draw_string(UINT32 *framebuffer, UINTN fb_width, UINTN x, UINTN y, const char *str, UINT32 color);
#endif