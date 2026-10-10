//
// circle_rect_collision.cpp -- Accurate collision calculation between circle and axis-aligned rectangle
//          Three collision cases: collide with vertical face / collide with horizontal face / collide with rectangle corner
//          Corresponds to supplementary knowledge point "Accurate Collision Calculation" from Research & Development Practice-3
//
// Compile: g++ -g -std=gnu++11 circle_rect_collision.cpp -o circle_rect_collision.exe -leasyx -lgdi32 -lole32 -luuid
#include <conio.h>
#include <graphics.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <time.h>

const int   W = 850, H = 850;
const float PI = 3.14159265f;
const float BALL_R = 20.0f;     // Radius of the ball
const float EPS = 1e-4f;        // Epsilon tolerance for floating-point comparison
const float BALL_SPEED = 6.0f;  // Constant ball speed, convenient for observing reflection angle

// Collision type enumeration
enum HitType { HIT_NONE = 0, HIT_FACE_V, HIT_FACE_H, HIT_CORNER };

// Axis-aligned rectangle
struct Rect { float left, top, right, bottom; };

// Ball (circle)
struct Ball { float x, y, vx, vy; };

// Rectangles in scene
const int N_RECT = 4;
Rect rects[N_RECT] = {
    { 110, 170, 310, 370 },   // Top-left rectangle
    { 420, 110, 670, 230 },   // Top-right rectangle
    { 520, 360, 760, 610 },   // Middle-right rectangle
    { 160, 560, 360, 710 },   // Bottom-left rectangle
};

Ball ball = { 0, 0, 0, 0 };

// Record of last collision (for drawing and angle display)
HitType lastHit = HIT_NONE;
float   lastCx = 0, lastCy = 0;   // Contact point
float   lastNx = 0, lastNy = 0;   // Contact normal (unit vector pointing toward ball)
float   incAngle = 0, refAngle = 0;
int     hitFlash = 0;             // Remaining frames for collision highlight

// ---------------- Utility Functions ----------------
// Find closest point on rectangle to point (px,py): clamp point into rectangle bounds
void ClosestPointOnRect(const Rect& rc, float px, float py, float& qx, float& qy)
{
    qx = px; qy = py;
    if (qx < rc.left)   qx = rc.left;
    if (qx > rc.right)  qx = rc.right;
    if (qy < rc.top)    qy = rc.top;
    if (qy > rc.bottom) qy = rc.bottom;
}

// Circle-rectangle collision detection and response
// Returns contact point (cx,cy), normal (nx,ny), and collision type
HitType CollideCircleRect(Ball& b, const Rect& rc,
float& cx, float& cy, float& nx, float& ny)
{
    float qx, qy;
    ClosestPointOnRect(rc, b.x, b.y, qx, qy);
    float dx = b.x - qx, dy = b.y - qy;
    float d2 = dx * dx + dy * dy;

    if (d2 > BALL_R * BALL_R) return HIT_NONE;   // No collision

    float dist = std::sqrt(d2);
    HitType type;

    if (dist > EPS) {
        // Normal = unit vector from closest point toward circle center
        nx = dx / dist;
        ny = dy / dist;
        // Check whether contact point is rectangle corner: x and y both clamped to boundary
        bool atX = (qx <= rc.left + EPS) || (qx >= rc.right - EPS);
        bool atY = (qy <= rc.top  + EPS) || (qy >= rc.bottom - EPS);
        if (atX && atY) type = HIT_CORNER;   // Corner collision
        else if (atX)   type = HIT_FACE_V;   // Vertical face collision
        else            type = HIT_FACE_H;   // Horizontal face collision
    }
    else {
        // Circle center trapped inside rectangle (rare case): push out along axis with minimal penetration
        float pl = b.x - rc.left,  pr = rc.right - b.x;
        float pt = b.y - rc.top,   pb = rc.bottom - b.y;
        float m = pl; int axis = 0;
        if (pr < m) { m = pr; axis = 1; }
        if (pt < m) { m = pt; axis = 2; }
        if (pb < m) { m = pb; axis = 3; }

        if (axis == 0)      { nx = -1; ny = 0; qx = rc.left;  qy = b.y; type = HIT_FACE_V; }
        else if (axis == 1) { nx =  1; ny = 0; qx = rc.right; qy = b.y; type = HIT_FACE_V; }
        else if (axis == 2) { nx =  0; ny = -1; qy = rc.top;    qx = b.x; type = HIT_FACE_H; }
        else                { nx =  0; ny =  1; qy = rc.bottom; qx = b.x; type = HIT_FACE_H; }
    }

    // Incidence angle: angle between incoming velocity and surface normal (calculated before reflection)
    float spd = std::sqrt(b.vx * b.vx + b.vy * b.vy);
    float vn = b.vx * nx + b.vy * ny;          // Normal component of velocity (<0 means approaching surface)
    if (vn < 0.0f) {
        float tmp = spd * spd - vn * vn;       // Squared tangential component
        if (tmp < 0.0f) tmp = 0.0f;
        float tang = std::sqrt(tmp);
        incAngle = std::atan2(tang, -vn) * 180.0f / PI;
        refAngle = incAngle;                   // Perfect specular reflection (e=1): incidence angle = reflection angle
        // General specular reflection formula v' = v - 2(v·n)n
        // For vertical face n=(±1,0), formula simplifies to vx=-vx; horizontal face n=(0,±1) simplifies to vy=-vy
        b.vx -= 2.0f * vn * nx;
        b.vy -= 2.0f * vn * ny;
    }

    // Push circle center to exactly BALL_R away from surface, prevent sticking / tunneling
    b.x = qx + nx * BALL_R;
    b.y = qy + ny * BALL_R;
    cx = qx; cy = qy;
    return type;
}

// Update ball position and handle all collisions
void UpdateBall()
{
    // Sub-stepping: split per-frame movement into small segments to avoid tunneling at high speed
    // This is the key for "accurate collision calculation"
    float spd = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
    int steps = (int)(spd / (BALL_R * 0.4f)) + 1;
    float sx = ball.vx / steps, sy = ball.vy / steps;

    for (int s = 0; s < steps; ++s) {
        ball.x += sx;
        ball.y += sy;

        for (int i = 0; i < N_RECT; ++i) {
            float cx, cy, nx, ny;
            HitType t = CollideCircleRect(ball, rects[i], cx, cy, nx, ny);
            if (t != HIT_NONE) {
                lastHit = t;
                lastCx = cx; lastCy = cy;
                lastNx = nx; lastNy = ny;
                hitFlash = 40;
            }
        }

        // Window boundary collision
        if (ball.x < BALL_R)          { ball.x = BALL_R;      ball.vx = -ball.vx; }
        else if (ball.x > W - BALL_R) { ball.x = W - BALL_R;  ball.vx = -ball.vx; }
        if (ball.y < BALL_R)          { ball.y = BALL_R;      ball.vy = -ball.vy; }
        else if (ball.y > H - BALL_R) { ball.y = H - BALL_R;  ball.vy = -ball.vy; }
    }
}

// Reset ball to initial position and assign random moving direction
void ResetBall()
{
    ball.x = W * 0.5f;
    ball.y = 60.0f;
    float a = (rand() % 360) * PI / 180.0f;
    ball.vx = std::cos(a) * BALL_SPEED;
    ball.vy = std::sin(a) * BALL_SPEED;
    lastHit = HIT_NONE;
    incAngle = refAngle = 0.0f;
    hitFlash = 0;
}

const TCHAR* HitName(HitType t)
{
    switch (t) {
        case HIT_FACE_V: return _T("vertical face");
        case HIT_FACE_H: return _T("horizontal face");
        case HIT_CORNER: return _T("CORNER / vertex");
        default:         return _T("-");
    }
}

void Draw()
{
    // Draw rectangles
    for (int i = 0; i < N_RECT; ++i) {
        setfillcolor(RGB(225, 228, 235));
        setlinecolor(RGB(150, 155, 170));
        fillrectangle((int)rects[i].left, (int)rects[i].top,
                      (int)rects[i].right, (int)rects[i].bottom);
    }

    // Draw contact point and normal (briefly shown after collision)
    if (hitFlash > 0 && lastHit != HIT_NONE) {
        setlinecolor(YELLOW);
        line((int)lastCx, (int)lastCy,
             (int)(lastCx + lastNx * 70), (int)(lastCy + lastNy * 70));
        setfillcolor(RGB(255, 200, 0));
        if (lastHit == HIT_CORNER)
            fillrectangle((int)lastCx - 4, (int)lastCy - 4,
                          (int)lastCx + 4, (int)lastCy + 4);   // Use square marker for corner
        else
            fillcircle((int)lastCx, (int)lastCy, 4);
    }

    // Draw ball
    setfillcolor(RGB(235, 70, 70));
    setlinecolor(RGB(180, 40, 40));
    fillcircle((int)ball.x, (int)ball.y, (int)BALL_R);

    // Draw velocity vector (green), visualize incoming/outgoing direction
    setlinecolor(RGB(120, 220, 120));
    line((int)ball.x, (int)ball.y,
         (int)(ball.x + ball.vx * 8), (int)(ball.y + ball.vy * 8));

    // Text rendering
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);
    settextstyle(16, 0, _T("Arial"));
    outtextxy(10, 10, _T("ESC quit   SPACE pause   R restart"));

    TCHAR buf[96];
    _stprintf_s(buf, _T("hit: %s"), HitName(lastHit));
    outtextxy(10, 34, buf);
    _stprintf_s(buf, _T("incidence=%.1f  reflection=%.1f (deg, vs normal)"), incAngle, refAngle);
    outtextxy(10, 56, buf);
}

int main()
{
    initgraph(W, H);
    setbkcolor(RGB(18, 18, 28));
    cleardevice();
    srand((unsigned int)time(NULL));
    ResetBall();
    bool paused = false;
    BeginBatchDraw();

    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 27) break;                       // ESC to quit
            if (ch == 'r' || ch == 'R') ResetBall();   // R to restart
            if (ch == ' ') paused = !paused;           // Space to toggle pause
        }
        if (!paused) UpdateBall();
        if (hitFlash > 0) hitFlash--;
        Draw();
        FlushBatchDraw();
        cleardevice();
        Sleep(16);   // ~60 FPS
    }
    EndBatchDraw();
    closegraph();
    return 0;
}
