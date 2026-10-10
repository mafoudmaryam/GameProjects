//
// triangle_bounce.cpp -- A solid triangle at the bottom, a small circle falls freely above.
// When colliding with the triangle, it bounces following the law of incidence and reflection.
// Compile: g++ -g -std=gnu++11 triangle_bounce.cpp -o triangle_bounce.exe -leasyx -lgdi32 -lole32 -luuid
#include <conio.h>
#include <graphics.h>
#include <cmath>
#include <cstdio>

const int W = 850;   // Window width
const int H = 850;   // Window height

// ---------------- Scene Parameters ----------------
const float BASE_Y    = H - 120.0f;   // Y coordinate of triangle base
const float TRI_H     = 220.0f;       // Triangle height
const float TRI_HALF  = 150.0f;       // Half width of triangle base
const float BALL_R    = 20.0f;        // Radius of small ball
const float GRAVITY   = 0.35f;        // Gravitational acceleration per frame
const float BOUNCE_DAMP = 0.85f;      // Bounce energy retention factor (<1 means energy loss)
const float PI        = 3.14159265f;
const float START_X   = W / 2.0f + 80.0f; // Spawn position (slightly right, lands on the slope)
const float START_Y   = 40.0f;

struct Vec2 { float x, y; };

// Three vertices of triangle (pointing upward, placed at bottom)
const Vec2 TRI[3] = {
    { W / 2.0f,            BASE_Y - TRI_H }, // Top vertex
    { W / 2.0f - TRI_HALF, BASE_Y        },  // Bottom-left vertex
    { W / 2.0f + TRI_HALF, BASE_Y        }   // Bottom-right vertex
};

// Find the closest point q on line segment ab to point p(px, py)
void ClosestPointOnSeg(float px, float py, float ax, float ay, float bx, float by,
float& qx, float& qy)
{
    float dx = bx - ax, dy = by - ay;
    float len2 = dx * dx + dy * dy;
    // Clamp projection parameter t to [0,1]: get closest point on segment, not infinite line
    float t = (len2 > 0.0f) ? ((px - ax) * dx + (py - ay) * dy) / len2 : 0.0f;
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;

    qx = ax + t * dx;
    qy = ay + t * dy;
}

// 2D cross product (b-a) cross (c-a), used to test whether a point lies inside triangle
static float Cross2D(float ax, float ay, float bx, float by, float cx, float cy)
{
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

// Check if point (px, py) is inside triangle (cross product same sign test: all edges must have same sign)
bool PointInTriangle(float px, float py)
{
    float d1 = Cross2D(TRI[0].x, TRI[0].y, TRI[1].x, TRI[1].y, px, py);
    float d2 = Cross2D(TRI[1].x, TRI[1].y, TRI[2].x, TRI[2].y, px, py);
    float d3 = Cross2D(TRI[2].x, TRI[2].y, TRI[0].x, TRI[0].y, px, py);

    bool hasNeg = (d1 < 0.0f) || (d2 < 0.0f) || (d3 < 0.0f);
    bool hasPos = (d1 > 0.0f) || (d2 > 0.0f) || (d3 > 0.0f);
    return !(hasNeg && hasPos);
}

// Detect collision between circle (cx,cy) and triangle.
// If collided, return surface normal (nx, ny) pointing toward circle center
// and distance from center to nearest surface point (dist)
// Used to push ball away from surface and prevent sticking / tunneling
bool GetContactNormal(float cx, float cy, float& nx, float& ny, float& dist)
{
    // 1) Circle center is inside triangle: pick nearest edge as contact surface
    if (PointInTriangle(cx, cy)) {
        float best = 1e30f;
        for (int i = 0; i < 3; ++i) {
            const Vec2& a = TRI[i];
            const Vec2& b = TRI[(i + 1) % 3];
            float qx, qy;
            ClosestPointOnSeg(cx, cy, a.x, a.y, b.x, b.y, qx, qy);
            float dx = cx - qx, dy = cy - qy;
            float d = std::sqrt(dx * dx + dy * dy);
            if (d < best) { best = d; nx = dx; ny = dy; }
        }
        dist = best;
        if (dist < 1e-6f) { nx = 0.0f; ny = -1.0f; }   // Fallback for degenerate case
        else { nx /= dist; ny /= dist; }
        return true;
    }

    // 2) Circle center outside triangle: find nearest edge with distance <= radius
    float best = 1e30f;
    bool hit = false;
    for (int i = 0; i < 3; ++i) {
        const Vec2& a = TRI[i];
        const Vec2& b = TRI[(i + 1) % 3];
        float qx, qy;
        ClosestPointOnSeg(cx, cy, a.x, a.y, b.x, b.y, qx, qy);
        float dx = cx - qx, dy = cy - qy;
        float d = std::sqrt(dx * dx + dy * dy);
        if (d <= BALL_R && d < best) { best = d; nx = dx; ny = dy; hit = true; }
    }
    if (hit) {
        dist = best;
        if (dist < 1e-6f) { nx = 0.0f; ny = -1.0f; }
        else { nx /= dist; ny /= dist; }
        return true;
    }
    return false;
}

// Draw solid triangle
void DrawTriangle()
{
    POINT pts[3] = {
        { (long)TRI[0].x, (long)TRI[0].y },
        { (long)TRI[1].x, (long)TRI[1].y },
        { (long)TRI[2].x, (long)TRI[2].y }
    };
    setfillcolor(RGB(70, 130, 200));
    setlinecolor(RGB(40, 90, 160));
    fillpolygon(pts, 3); // Solid polygon with border
}

int main()
{
    initgraph(W, H);
    setbkcolor(RGB(0, 120, 0));
    cleardevice();

    float bx = START_X, by = START_Y; // Ball position
    float vx = 0.0f, vy = 0.0f;       // Velocity (contains horizontal component for reflection)
    float incAngle = 0.0f;            // Last incidence angle (degree, relative to normal)
    float refAngle = 0.0f;            // Last reflection angle (degree, relative to normal)

    BeginBatchDraw();
    while (true) {
        // ---------------- Input ----------------
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 27) break;                          // ESC to exit
            if (ch == 'r' || ch == 'R') {                 // R to restart fall
                bx = START_X; by = START_Y; vx = 0.0f; vy = 0.0f;
                incAngle = refAngle = 0.0f;
            }
        }

        // ---------------- Physics Update ----------------
        vy += GRAVITY;                 // Apply gravity
        float nextX = bx + vx;
        float nextY = by + vy;
        float nx, ny, dist;

        if (GetContactNormal(nextX, nextY, nx, ny, dist)) {
            float vn = vx * nx + vy * ny;   // Normal component of velocity (<0 means approaching surface)
            if (vn < 0.0f) {
                // Incidence angle = angle between incoming velocity and surface normal
                float speedIn = std::sqrt(vx * vx + vy * vy);
                float tmp = speedIn * speedIn - vn * vn;   // Square of tangential component
                if (tmp < 0.0f) tmp = 0.0f;
                float tang = std::sqrt(tmp);
                float theta = std::atan2(tang, -vn) * 180.0f / PI; // -vn = |vn|
                incAngle = theta;
                refAngle = theta;          // Law of reflection: incidence angle = reflection angle

                // Specular reflection formula (e=1): v' = v - 2(v·n)n
                float rx = vx - 2.0f * vn * nx;
                float ry = vy - 2.0f * vn * ny;
                vx = rx * BOUNCE_DAMP;     // Multiply by damping factor for energy loss
                vy = ry * BOUNCE_DAMP;
            }
            // Push ball along normal away from surface so center stays exactly radius away, avoid sticking/tunneling
            bx = nextX + nx * (BALL_R - dist);
            by = nextY + ny * (BALL_R - dist);
        }
        else {
            bx = nextX;
            by = nextY;
        }

        // Border bounce: keep ball inside window for repeated observation
        if (bx < BALL_R)          { bx = BALL_R;       vx = -vx * BOUNCE_DAMP; }
        else if (bx > W - BALL_R) { bx = W - BALL_R;   vx = -vx * BOUNCE_DAMP; }
        if (by < BALL_R)          { by = BALL_R;       vy = -vy * BOUNCE_DAMP; }
        else if (by > H - BALL_R) { by = H - BALL_R;   vy = -vy * BOUNCE_DAMP; }

        // ---------------- Rendering ----------------
        DrawTriangle();
        setfillcolor(RED);
        fillcircle((int)bx, (int)by, (int)BALL_R);

        // Draw current velocity vector starting from ball center, visualize incoming/outgoing direction
        setlinecolor(YELLOW);
        line((int)bx, (int)by, (int)(bx + vx * 6.0f), (int)(by + vy * 6.0f));

        settextcolor(WHITE);
        setbkmode(TRANSPARENT);
        settextstyle(16, 0, _T("Arial"));
        outtextxy(10, 10, _T("ESC: quit    R: restart"));

        TCHAR buf[80];
        _stprintf(buf, _T("incidence=%.1f  reflection=%.1f (deg, vs normal)"), incAngle, refAngle);
        outtextxy(10, 40, buf);

        FlushBatchDraw();
        cleardevice();
        Sleep(16); // ~60 FPS
    }
    EndBatchDraw();
    closegraph();
    return 0;
}
