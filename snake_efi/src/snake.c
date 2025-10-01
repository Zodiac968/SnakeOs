#include <snake.h>

UINT64 seed = 12740912551;
UINT32 random()
{
        seed = seed * 17539067133617 + 123;
        seed = seed >> 32;
        return (UINT32)seed;
}

UINT32 *framebuffer;
UINTN width;
UINTN height;

UINTN gameW;
UINTN gameH;
UINTN xOffset = 128;
UINTN yOffset = 0;
UINTN snakeCols;
UINTN snakeRows;
UINTN cellSize = 32;

UINT32 snakeLength = 5;
UINTN xvel = 1;
UINTN yvel = 0;
UINT32 snakePos[100];
UINT32 applePos = 0;

CHAR16 appleString[16];

VOID toString(UINT32 n, CHAR16 *str)
{
        UINTN i = 0;
        if (n == 0)
        {
                str[0] = L'0';
                str[1] = L'\0';
        }
        else
        {
                CHAR16 temp[16];
                while (n > 0)
                {
                        temp[i++] = L'0' + (n % 10);
                        n /= 10;
                }
                for (UINTN j = 0; j < i; j++)
                {
                        str[j] = temp[i - j - 1];
                }
                str[i] = '\0';
        }
        return;
}

VOID drawBox(UINTN x1, UINTN y1, UINTN x2, UINTN y2, UINT32 col)
{
        x1 += xOffset;
        y1 += yOffset;
        x2 += xOffset;
        y2 += yOffset;
        for (UINTN a = x1; a < x2; a++)
        {
                for (UINTN b = y1; b < y2; b++)
                {
                        // if (a == x1 || a == x2-1 || b == y1 || b == y2-1) framebuffer[b * width + a] = 0xFFFFFF;
                        // else framebuffer[b * width + a] = col;
                        framebuffer[b * width + a] = col;
                }
        }
}

VOID drawCell(UINTN x, UINTN y, UINT32 col, UINTN cellSize)
{
        UINTN x1 = x * cellSize;
        UINTN y1 = y * cellSize;
        drawBox(x1, y1, x1 + cellSize, y1 + cellSize, col);
}

VOID snakeInitialise(UINT32 *fb, UINTN w, UINTN h)
{
        framebuffer = fb;
        width = w;
        height = h;
        gameW = (width * 3 / 5) / cellSize * cellSize;
        gameH = (height * 3 / 5) / cellSize * cellSize;
        snakeCols = gameW / cellSize;
        snakeRows = gameH / cellSize;
        snakeLength = 5;
        xOffset = 128 + (width - gameW) / 2;
        yOffset = 0 + (height - gameH) / 2;
        xvel = 1;
        yvel = 0;
        applePos = random() % (snakeCols * snakeRows);
        for (UINTN i = 0; i < 100; i++)
        {
                if (i < snakeLength)
                        snakePos[i] = 5 + i;
                else
                        snakePos[i] = 0;
        }
        drawBox(-3, -3, 3 + gameW, 3 + gameH, 0xFFFFFF);
        drawBox(-2, -2, 2 + gameW, 2 + gameH, 0x000000);
        drawBox(-1, -1, 1 + gameW, 1 + gameH, 0xFFFFFF);
        drawBox(0, 0, gameW, gameH, 0x000000);
        draw_string(framebuffer, width, xOffset, yOffset - 32, L"Apples Eaten: ", 0xFFFFFF);
        toString(snakeLength - 5, appleString);
        draw_string(framebuffer, width, xOffset + string_length(L"Apples Eaten: ") * 8, yOffset - 32, appleString, 0xFFFFFF);
}

BOOLEAN snakeUpdate(EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ci)
{
        UINT32 snakeColor = 0x00FF00; // RGB
        UINT32 appleColor = 0xFF0000;
        EFI_INPUT_KEY key;
        if (ci->ReadKeyStroke(ci, &key) == EFI_SUCCESS)
        {
                CHAR16 ch = key.UnicodeChar;
                if (ch == 'w' || ch == 'W')
                {
                        if (yvel == 0)
                        {
                                xvel = 0;
                                yvel = -1;
                        }
                }
                else if (ch == 'a' || ch == 'A')
                {
                        if (xvel == 0)
                        {
                                xvel = -1;
                                yvel = 0;
                        }
                }
                else if (ch == 's' || ch == 'S')
                {
                        if (yvel == 0)
                        {
                                xvel = 0;
                                yvel = 1;
                        }
                }
                else if (ch == 'd' || ch == 'D')
                {
                        if (xvel == 0)
                        {
                                xvel = 1;
                                yvel = 0;
                        }
                }
        }
        UINTN currentCol = snakePos[snakeLength - 1] % snakeCols;
        UINTN currentRow = snakePos[snakeLength - 1] / snakeCols;
        if (xvel == 1 && currentCol == snakeCols - 1)
                return FALSE;
        if (xvel == -1 && currentCol == 0)
                return FALSE;
        if (yvel == 1 && currentRow == snakeRows - 1)
                return FALSE;
        if (yvel == -1 && currentRow == 0)
                return FALSE;
        drawCell(snakePos[0] % snakeCols, snakePos[0] / snakeCols, 0x00000000, cellSize);
        UINT32 nextPos = snakePos[snakeLength - 1] + xvel + yvel * snakeCols;
        currentCol += xvel;
        currentRow += yvel;
        BOOLEAN collide = FALSE;
        for (UINTN i = 0; i < snakeLength - 1; i++)
        {
                if (snakePos[i] == nextPos)
                {
                        collide = TRUE;
                        return FALSE;
                }
        }
        if (collide)
                return FALSE;
        if (nextPos == applePos)
        {
                snakePos[snakeLength++] = applePos;
                BOOLEAN overlap = TRUE;
                while (overlap)
                {
                        applePos = random() % (snakeCols * snakeRows);
                        overlap = FALSE;
                        for (UINTN i = 0; i < snakeLength; i++)
                        {
                                if (snakePos[i] == applePos)
                                {
                                        overlap = TRUE;
                                        break;
                                }
                        }
                }
                draw_string(framebuffer, width, xOffset, yOffset - 32, L"Apples Eaten: ", 0xFFFFFF);
                toString(snakeLength - 5, appleString);
                draw_string(framebuffer, width, xOffset + string_length(L"Apples Eaten: ") * 8, yOffset - 32, appleString, 0xFFFFFF);
        }
        else
        {
                for (UINTN i = 0; i < snakeLength - 1; i++)
                {
                        snakePos[i] = snakePos[i + 1];
                }
                snakePos[snakeLength - 1] = nextPos;
        }

        for (UINTN i = 0; i < snakeLength; i++)
        {
                drawCell(snakePos[i] % snakeCols, snakePos[i] / snakeCols, snakeColor, cellSize);
        }
        drawBox(currentCol * cellSize + cellSize * 1 / 4, currentRow * cellSize + cellSize * 1 / 4, currentCol * cellSize + cellSize * 3 / 4, currentRow * cellSize + cellSize * 3 / 4, 0xFFFFFF);
        drawCell(applePos % snakeCols, applePos / snakeCols, appleColor, cellSize);
        return TRUE;
}

VOID snakeClear()
{
        drawBox(-3, -35, 3 + gameW, 3 + gameH, 0x000000);
}