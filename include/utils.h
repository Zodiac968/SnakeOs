#ifndef UTILS_H
#define UTILS_H

#include <efi.h>

UINTN string_length(CHAR16 *str);
void clrstr(CHAR16 *str);
void concat(CHAR16 *dest, const CHAR16 *src);
void utoa(UINTN value, CHAR16 *buffer);
BOOLEAN streq(CHAR16 *a, CHAR16 *b);
void backspace(CHAR16 *str);

#endif
