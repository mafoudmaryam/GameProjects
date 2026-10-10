#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 400
#define GROUND_Y 350

int main()
{
    srand((unsigned)time(NULL));

    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();

    // ---- Ball (player) ----
    int x_ball = 100;
    double y_ball = GROUND_Y - 15;   // resting on the ground
    int r = 15;                       // ball is treated as a square, r = half its size
    double vy = 0;                    // vertical speed
    double gravity = 1.0;
    bool jumping = false;

    // ---- Pillar (obstacle) ----
    int pillar_width = 20;
    int x_pillar = WIDTH;
    int pillar_height = rand() % 60 + 30;   // random height 30-90
    int pillar_speed = rand() % 5 + 4;      // random speed 4-9
    bool counted = false;                    // scored for this pillar yet?

    int score = 0;
    bool gameOver = false;

    while (!gameOver)
    {
        // ---- Input ----
        if ((GetAsyncKeyState(VK_SPACE) & 0x8000) && !jumping)
        {
            jumping = true;
            vy = -14;
        }

        // ---- Ball physics ----
        if (jumping)
        {
            y_ball += vy;
            vy += gravity;
            if (y_ball >= GROUND_Y - r)
            {
                y_ball = GROUND_Y - r;
                vy = 0;
                jumping = false;
            }
        }

        // ---- Move pillar ----
        x_pillar -= pillar_speed;
        if (x_pillar + pillar_width < 0)
        {
            x_pillar = WIDTH;
            pillar_height = rand() % 60 + 30;
            pillar_speed = rand() % 5 + 4;
            counted = false;
        }

        // ---- Score when pillar fully passes the ball ----
        if (!counted && x_pillar + pillar_width < x_ball - r)
        {
            score++;
            counted = true;
        }

        // ---- Collision detection ----
        int pillar_top = GROUND_Y - pillar_height;
        bool overlapX = (x_ball + r > x_pillar) && (x_ball - r < x_pillar + pillar_width);
        bool overlapY = (y_ball + r > pillar_top);   // ball hasn't jumped above the pillar
        if (overlapX && overlapY)
        {
            gameOver = true;
        }

        // ---- Draw ----
        setbkcolor(WHITE);
        cleardevice();

        setlinecolor(BLACK);
        line(0, GROUND_Y, WIDTH, GROUND_Y);

        setfillcolor(BLUE);
        solidrectangle(x_ball - r, (int)y_ball - r, x_ball + r, (int)y_ball + r);

        setfillcolor(RED);
        solidrectangle(x_pillar, pillar_top, x_pillar + pillar_width, GROUND_Y);

        settextcolor(BLACK);
        settextstyle(24, 0, "Consolas");
        char text[50];
        sprintf(text, "Score: %d", score);
        outtextxy(20, 20, text);

        FlushBatchDraw();
        Sleep(15);
    }

    setbkcolor(WHITE);
    cleardevice();
    settextcolor(RED);
    settextstyle(30, 0, "Consolas");
    char text2[80];
    sprintf(text2, "Game Over! Score: %d", score);
    outtextxy(WIDTH/2 - 150, HEIGHT/2, text2);
    FlushBatchDraw();

    _getch();
    EndBatchDraw();
    closegraph();
    return 0;
}