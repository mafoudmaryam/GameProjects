// hello_shapes.cpp -- Lesson 1: open a window, use colors, draw shapes, then animate one.
// Compile: g++ hello_shapes.cpp -o hello_shapes.exe -leasyx -lgdi32 -lole32 -luuid
#include <conio.h>
#include <graphics.h>
#include <windows.h>

#define WIDTH 640
#define HEIGHT 480

int main()
{
    initgraph(WIDTH, HEIGHT);           // 1. Create the game screen

    // ---- Part A: a static picture ----
    setbkcolor(RGB(30, 30, 60));        // 2. Background color (R, G, B each 0~255)
    cleardevice();                      //    Repaint the whole screen with it

    setlinecolor(WHITE);                // 3. Outline color
    line(0, 400, WIDTH, 400);           //    The ground

    setfillcolor(RGB(255, 200, 0));     // 4. Fill color
    solidcircle(540, 80, 40);           //    A sun (filled, no outline)

    setfillcolor(RGB(200, 60, 60));
    fillrectangle(100, 300, 220, 400);  //    A house (filled + outline)

    setlinecolor(RGB(0, 200, 255));
    circle(160, 260, 30);               //    An empty circle

    settextcolor(WHITE);
    setbkmode(TRANSPARENT);             //    Text without a background box
    outtextxy(10, 10, _T("Static picture. Press any key to animate..."));

    _getch();                           // Wait for a key

    // ---- Part B: animation = draw, wait, clear, move, repeat ----
    int x = 40, y = 360, r = 30;
    int vx = 4;

    BeginBatchDraw();                   // Draw off-screen first: no flicker
    while (!_kbhit())                   // Run until a key is pressed
    {
        cleardevice();                  // Erase the previous frame

        setlinecolor(WHITE);
        line(0, 400, WIDTH, 400);
        setfillcolor(RGB(255, 80, 80));
        solidcircle(x, y, r);
        outtextxy(10, 10, _T("Animation. Press any key to quit."));

        x += vx;                        // Move
        if (x > WIDTH - r || x < r)     // Hit a wall? turn around
            vx = -vx;

        FlushBatchDraw();               // Show the finished frame
        Sleep(16);                      // Wait ~16 ms (about 60 frames per second)
    }
    EndBatchDraw();

    closegraph();
    return 0;
}
