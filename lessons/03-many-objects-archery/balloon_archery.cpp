#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 400
#define GROUND_Y 350
#define MAX_ARROWS 30
#define NUM_BALLOONS 5

struct Arrow {
    double x, y, vx, vy;
    bool active;
};

struct Balloon {
    double x, y, vx, vy;
    int r;
    bool onBar;
    bool active;
    COLORREF color;
};

void drawClouds()
{
    setfillcolor(RGB(255, 255, 255));
    int cx[3] = {150, 420, 650};
    int cy[3] = {60, 40, 70};
    for (int i = 0; i < 3; i++)
    {
        solidcircle(cx[i], cy[i], 16);
        solidcircle(cx[i] + 18, cy[i] + 5, 20);
        solidcircle(cx[i] - 18, cy[i] + 5, 14);
    }
}

int main()
{
    srand((unsigned)time(NULL));
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();

    int px = 50, py = GROUND_Y;

    Arrow arrows[MAX_ARROWS];
    for (int i = 0; i < MAX_ARROWS; i++) arrows[i].active = false;

    COLORREF palette[5] = {
        RGB(235, 64, 52), RGB(240, 160, 30), RGB(180, 70, 220),
        RGB(40, 150, 230), RGB(250, 200, 30)
    };

    Balloon balloons[NUM_BALLOONS];
    for (int i = 0; i < NUM_BALLOONS - 1; i++)
    {
        balloons[i].x = rand() % 600 + 100;
        balloons[i].y = GROUND_Y - (rand() % 250 + 50);
        balloons[i].vx = 0;
        balloons[i].vy = -(rand() % 2 + 1);
        balloons[i].r = 15;
        balloons[i].onBar = false;
        balloons[i].active = true;
        balloons[i].color = palette[i];
    }
    int barY = 80, barLeft = 120, barRight = 700;
    balloons[NUM_BALLOONS - 1].x = barLeft;
    balloons[NUM_BALLOONS - 1].y = barY - 15;
    balloons[NUM_BALLOONS - 1].vx = 3;
    balloons[NUM_BALLOONS - 1].vy = 0;
    balloons[NUM_BALLOONS - 1].r = 15;
    balloons[NUM_BALLOONS - 1].onBar = true;
    balloons[NUM_BALLOONS - 1].active = true;
    balloons[NUM_BALLOONS - 1].color = palette[4];

    int score = 0;
    bool prevSpace = false;
    bool running = true;

    while (running)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) running = false;

        bool spaceNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        if (spaceNow && !prevSpace)
        {
            for (int i = 0; i < MAX_ARROWS; i++)
            {
                if (!arrows[i].active)
                {
                    arrows[i].x = px + 15;
                    arrows[i].y = py - 15;
                    arrows[i].vx = 12;
                    arrows[i].vy = -7;
                    arrows[i].active = true;
                    break;
                }
            }
        }
        prevSpace = spaceNow;

        for (int i = 0; i < MAX_ARROWS; i++)
        {
            if (!arrows[i].active) continue;
            arrows[i].x += arrows[i].vx;
            arrows[i].y += arrows[i].vy;
            if (arrows[i].x > WIDTH || arrows[i].y < 0) arrows[i].active = false;
        }

        for (int i = 0; i < NUM_BALLOONS; i++)
        {
            if (!balloons[i].active) continue;
            if (balloons[i].onBar)
            {
                balloons[i].x += balloons[i].vx;
                if (balloons[i].x < barLeft || balloons[i].x > barRight)
                    balloons[i].vx = -balloons[i].vx;
            }
            else
            {
                balloons[i].y += balloons[i].vy;
                if (balloons[i].y < 0)
                {
                    balloons[i].x = rand() % 600 + 100;
                    balloons[i].y = GROUND_Y;
                    balloons[i].vy = -(rand() % 2 + 1);
                }
            }
        }

        for (int i = 0; i < MAX_ARROWS; i++)
        {
            if (!arrows[i].active) continue;
            for (int j = 0; j < NUM_BALLOONS; j++)
            {
                if (!balloons[j].active) continue;
                double dx = arrows[i].x - balloons[j].x;
                double dy = arrows[i].y - balloons[j].y;
                double dist = sqrt(dx * dx + dy * dy);
                if (dist < balloons[j].r + 4)
                {
                    arrows[i].active = false;
                    score++;
                    if (balloons[j].onBar)
                        balloons[j].x = barLeft;
                    else
                    {
                        balloons[j].x = rand() % 600 + 100;
                        balloons[j].y = GROUND_Y;
                        balloons[j].vy = -(rand() % 2 + 1);
                    }
                }
            }
        }

        // ---- Draw background ----
        setbkcolor(RGB(135, 206, 250));   // sky blue
        cleardevice();
        drawClouds();

        // ground: dirt band + grass strip on top
        setfillcolor(RGB(139, 90, 43));
        solidrectangle(0, GROUND_Y, WIDTH, HEIGHT);
        setfillcolor(RGB(50, 160, 60));
        solidrectangle(0, GROUND_Y, WIDTH, GROUND_Y + 10);

        // wooden bar
        setfillcolor(RGB(120, 70, 30));
        solidrectangle(barLeft - 15, barY - 6, barRight + 15, barY + 6);

        // player: body + bow arc
        setfillcolor(RGB(70, 130, 180));
        solidcircle(px, py - 20, 14);
        setlinecolor(RGB(90, 60, 30));
        setlinestyle(PS_SOLID, 3);
        arc(px - 5, py - 40, px + 25, py, 300, 60);

        // arrows: shaft + triangular head
        for (int i = 0; i < MAX_ARROWS; i++)
        {
            if (!arrows[i].active) continue;
            int hx = (int)arrows[i].x, hy = (int)arrows[i].y;
            int tx = (int)(arrows[i].x - arrows[i].vx), ty = (int)(arrows[i].y - arrows[i].vy);
            setlinecolor(RGB(90, 60, 30));
            setlinestyle(PS_SOLID, 2);
            line(tx, ty, hx, hy);
            setfillcolor(RGB(60, 60, 60));
            solidcircle(hx, hy, 3);
        }

        // balloons: body + highlight + string
        for (int i = 0; i < NUM_BALLOONS; i++)
        {
            if (!balloons[i].active) continue;
            int bx = (int)balloons[i].x, by = (int)balloons[i].y;
            setlinecolor(RGB(120, 120, 120));
            setlinestyle(PS_SOLID, 1);
            line(bx, by + balloons[i].r, bx, by + balloons[i].r + 18);
            setfillcolor(balloons[i].color);
            solidcircle(bx, by, balloons[i].r);
            setfillcolor(RGB(255, 255, 255));
            solidcircle(bx - 5, by - 5, 4);
        }

        // score panel
        setfillcolor(RGB(255, 255, 255));
        solidrectangle(10, 10, 170, 45);
        setlinecolor(RGB(0, 0, 0));
        rectangle(10, 10, 170, 45);
        settextcolor(RGB(20, 20, 20));
        settextstyle(22, 0, "Consolas");
        char text[50];
        sprintf(text, "Score: %d", score);
        outtextxy(20, 16, text);

        FlushBatchDraw();
        Sleep(15);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}