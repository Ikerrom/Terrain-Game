#ifndef GAMECONSOLEENGINE_CPP
#define GAMECONSOLEENGINE_CPP
#include <iostream>
#include <cmath>
#include <Windows.h>
using namespace std;

int WIDTH;
int HEIGTH;
int mouseX;
int mouseY;
bool leftpressed = false;
bool rigthpressed = false;
int keynum;
CHAR_INFO *screen;
HANDLE hConsole;
HANDLE hConsoleIn;
SMALL_RECT windowrect;
string cname;
CONSOLE_SCREEN_BUFFER_INFOEX info;
CONSOLE_FONT_INFOEX cfi;

void SetWindow(int Width, int Height, int fWidth, int fHeight)
{
    WIDTH = Width;
    HEIGTH = Height;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

    windowrect = {0, 0, 1, 1};
    SetConsoleWindowInfo(hConsole, TRUE, &windowrect);

    COORD coord = {(short)Width, (short)Height};
    SetConsoleScreenBufferSize(hConsole, coord);
    SetConsoleActiveScreenBuffer(hConsole);

    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = fWidth;
    cfi.dwFontSize.Y = fHeight;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hConsole, false, &cfi);

    windowrect = {0, 0, (short)(Width - 1), (short)(Height - 1)};
    SetConsoleWindowInfo(hConsole, TRUE, &windowrect);

    SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

    screen = new CHAR_INFO[Width * Height];
    memset(screen, 0, sizeof(CHAR_INFO) * Width * Height);
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandle, TRUE);
}

void SetConsoleName(char* chararray){
    SetConsoleTitleA(chararray);
}

void SetConsoleColor(int index, int R, int G, int B){
    info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hConsole, &info);
    info.ColorTable[index] = RGB(R, G, B);
    SetConsoleScreenBufferInfoEx(hConsole, &info);
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void Draw(int x, int y, int bgColor)
{
    if (x < WIDTH && x >= 0 && y < HEIGTH && y >= 0)
    {
        int i = (int)y * (int)WIDTH + (int)x;
        screen[i].Char.UnicodeChar = 0x2588;
        screen[i].Attributes = bgColor;
    }
}

void DrawString(int x, int y, string str, int bgColor)
{
    if (x < WIDTH && x >= 0 && y < HEIGTH && y >= 0)
    {
        for (size_t i = 0; i < str.size(); i++)
        {
            screen[i].Char.UnicodeChar = str[i];
            screen[i].Attributes = bgColor;
        }
    }
}

void DrawCircle(int x, int y, int radius, int bgColor)
{
    float d;
    float xk;
    float yi;
    for (int i = -radius; i <= radius; i++)
    {
        for (int k = -radius; k <= radius; k++)
        {
            xk = x + k;
            yi = y + i;
            d = sqrt(pow(x - (xk), 2) + pow(y - (yi), 2));
            if (d <= radius && d >= radius - 1)
            {
                Draw(xk, yi, bgColor);
            }
        }
    }
}

void DrawFilledCircle(int x, int y, int radius, int bgColor)
{
    float d;
    float xk;
    float yi;
    for (int i = -radius; i <= radius; i++)
    {
        for (int k = -radius; k <= radius; k++)
        {
            xk = x + k;
            yi = y + i;
            d = sqrt(pow(x - (xk), 2) + pow(y - (yi), 2));
            if (d <= radius)
            {
                Draw(xk, yi, bgColor);
            }
        }
    }
}

void DrawLine(int x, int y, int x2, int y2, int bgColor)
{
    float deltax = x2 - x;
    float deltay = y2 - y;
    float d = sqrt(pow(y2 - y, 2) + pow(x2 - x, 2));
    deltax /= d;
    deltay /= d;
    float dx = x;
    float dy = y;
    while (d > 1)
    {
        dx = dx + deltax;
        dy = dy + deltay;
        d = sqrt(pow(y2 - dy, 2) + pow(x2 - dx, 2));
        Draw(dx, dy, bgColor);
    }
}

void DrawToScreen()
{
    WriteConsoleOutput(hConsole, screen, {(short)WIDTH, (short)HEIGTH}, {0, 0}, &windowrect);
}

void Fill(int x, int y, int x2, int y2, int bgColor)
{
    for (int i = y; i < y2; i++)
    {
        for (int k = x; k < x2; k++)
        {
            Draw(k, i, bgColor);
        }
    }
}

void CheckConsoleInput()
{
    INPUT_RECORD irInBuf[128];
    DWORD cNumRead = 0;
    GetNumberOfConsoleInputEvents(hConsoleIn, &cNumRead);
    if (cNumRead > 0)
    {
        ReadConsoleInput(hConsoleIn, irInBuf, cNumRead, &cNumRead);
    }

    for (DWORD i = 0; i < cNumRead; i++)
    {
        if (irInBuf[i].EventType == MOUSE_EVENT)
        {
            if (irInBuf[i].Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                mouseX = irInBuf[i].Event.MouseEvent.dwMousePosition.X;
                mouseY = irInBuf[i].Event.MouseEvent.dwMousePosition.Y;
                if (leftpressed == false)
                {
                    leftpressed = true;
                }
            }
            else
            {
                if (leftpressed == true)
                {
                    leftpressed = false;
                }
            }

            if (irInBuf[i].Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
            {
                mouseX = irInBuf[i].Event.MouseEvent.dwMousePosition.X;
                mouseY = irInBuf[i].Event.MouseEvent.dwMousePosition.Y;
                if (rigthpressed == false)
                {
                    rigthpressed = true;
                }
            }
            else
            {
                if (rigthpressed == true)
                {
                    rigthpressed = false;
                }
            }

            if (irInBuf[i].Event.MouseEvent.dwButtonState == MOUSE_MOVE_RELATIVE)
            {
                mouseX = irInBuf[i].Event.MouseEvent.dwMousePosition.X;
                mouseY = irInBuf[i].Event.MouseEvent.dwMousePosition.Y;
            }

            
        }

        if (irInBuf[i].EventType == KEY_EVENT)
        {
            keynum = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;
        }
    }
}

#endif