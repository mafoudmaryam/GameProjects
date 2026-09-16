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
#define PI 3.14159265
#define MAX_TRAIL 80

int main()
{
    srand((unsigned)time(NULL));
    initgraph(WIDTH, HEIGHT);
    BeginBatchDraw();

    // ---- Cannon ----
    int cannonX = 60, cannonY = GROUND_Y;
    double angle = 45;
    double power = 20;

    // ---- Ball in flight ----
    bool ballFlying = false;
    double bx, by, bvx, bvy;
    double gravity = 0.35;
    int ballR = 8;

    // ---- Trail of circles along the flight path ----
    double trailX[MAX_TRAIL], trailY[MAX_TRAIL];
    int trailCount = 0;

    // ---- Moving target ----
    double targetY = 130;
    double targetX = 400;
    double targetLeft = 300, targetRight = 700;
    double targetSpeed = rand() % 4 + 3;
    int targetR = 18;

    int score = 0;
    int shots = 0;
    int hitFlash = 0;
    bool prevSpace = false;
    bool running = true;

    while (running)
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) running = false;

        // ---- Aim controls (only while no ball is flying) ----
        if (!ballFlying)
        {
            if (GetAsyncKeyState(VK_LEFT) & 0x8000)  angle += 1;
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) angle -= 1;
            if (angle > 85) angle = 85;
            if (angle < 5) angle = 5;

            if (GetAsyncKeyState(VK_UP) & 0x8000)   power += 0.3;
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) power -= 0.3;
            if (power > 35) power = 35;
            if (power < 8) power = 8;
        }

        // ---- Fire ----
        bool spaceNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
        if (spaceNow && !prevSpace && !ballFlying)
        {
            ballFlying = true;
            bx = cannonX;
            by = cannonY;
            bvx = power * cos(angle * PI / 180.0);
            bvy = -power * sin(angle * PI / 180.0);
            shots++;
            trailCount = 0;   // clear old trail for the new shot
        }
        prevSpace = spaceNow;

        // ---- Move target ----
        targetX += targetSpeed;
        if (targetX > targetRight || targetX < targetLeft)
        {
            targetSpeed = -targetSpeed;
            targetX += targetSpeed;
        }

        if (hitFlash > 0) hitFlash--;

        // ---- Move ball + collision detection ----
        if (ballFlying)
        {
            bx += bvx;
            by += bvy;
            bvy += gravity;

            // add current position to the trail (so it draws as a chain of circles)
            if (trailCount < MAX_TRAIL)
            {
                trailX[trailCount] = bx;
                trailY[trailCount] = by;
                trailCount++;
            }

            double dx = bx - targetX;
            double dy = by - targetY;
            double dist = sqrt(dx * dx + dy * dy);
            if (dist < targetR + 6)
            {
                score++;
                ballFlying = false;
                hitFlash = 30;
                Beep(1000, 100);
                targetSpeed = (rand() % 4 + 3) * (targetSpeed > 0 ? 1 : -1);
            }
            else if (by > GROUND_Y || bx > WIDTH || bx < 0)
            {
                ballFlying = false;   // missed
            }
        }

        // ---- Draw ----
        setbkcolor(RGB(200, 220, 255));
        cleardevice();

        setfillcolor(RGB(120, 80, 40));
        solidrectangle(0, GROUND_Y, WIDTH, HEIGHT);
        setfillcolor(RGB(60, 170, 70));
        solidrectangle(0, GROUND_Y, WIDTH, GROUND_Y + 8);

        // cannon barrel
        setlinecolor(RGB(40, 40, 40));
        setlinestyle(PS_SOLID, 6);
        int aimX = cannonX + (int)(40 * cos(angle * PI / 180.0));
        int aimY = cannonY - (int)(40 * sin(angle * PI / 180.0));
        line(cannonX, cannonY, aimX, aimY);
        setfillcolor(RGB(80, 80, 80));
        solidcircle(cannonX, cannonY, 12);

        // ---- Ball trail: chain of open circles, just like the sample ----
        setlinestyle(PS_SOLID, 1);
        setlinecolor(RGB(30, 30, 30));
        for (int i = 0; i < trailCount; i++)
        {
            circle((int)trailX[i], (int)trailY[i], ballR);
        }

        // target
        setfillcolor(RGB(220, 40, 40));
        solidcircle((int)targetX, (int)targetY, targetR);
        setfillcolor(RGB(255, 255, 255));
        solidcircle((int)targetX, (int)targetY, targetR / 3);

        // HUD
        setfillcolor(RGB(255, 255, 255));
        solidrectangle(10, 10, 230, 90);
        setlinecolor(RGB(0,0,0));
        rectangle(10, 10, 230, 90);
        settextcolor(RGB(20, 20, 20));
        settextstyle(20, 0, "Consolas");
        char text[80];
        sprintf(text, "Score: %d / %d shots", score, shots);
        outtextxy(20, 16, text);
        sprintf(text, "Angle: %.0f  Power: %.0f", angle, power);
        outtextxy(20, 42, text);
        outtextxy(20, 66, "Arrows: aim/power  Space: fire");

        if (hitFlash > 0)
        {
            settextcolor(RGB(220, 30, 30));
            settextstyle(40, 0, "Consolas");
            outtextxy(WIDTH / 2 - 60, 150, "HIT!");
        }

        FlushBatchDraw();
        Sleep(15);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}