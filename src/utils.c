#include <utils.h>

UINTN string_length(CHAR16 *str)
{
    UINTN count = 0;
    while (str[count] != L'\0')
    {
        count++;
    }
    return count;
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

VOID clrstr(CHAR16 *str)
{
    *str = '\0';
}

VOID backspace(CHAR16 *str)
{
    if (*str == L'\0' || !str)
        return;
    while (*str)
        str++;
    *(--str) = L'\0';
}

BOOLEAN streq(CHAR16 *a, CHAR16 *b)
{
    while (*a && *b)
    {
        if (*a != *b)
            return FALSE;
        a++;
        b++;
    }
    return (*a == *b); // both must be null
}
