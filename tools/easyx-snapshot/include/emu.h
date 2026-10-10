// Tiny EasyX emulator: records drawing calls as SVG so the real lesson code can be "screenshotted" on Linux.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>
typedef unsigned long COLORREF; typedef char TCHAR; typedef const char* LPCTSTR;
struct POINT { long x, y; };
#define _T(x) x
#define RGB(r,g,b) ((COLORREF)(((r)&255)|(((g)&255)<<8)|(((b)&255)<<16)))
#define BLACK 0x000000UL
#define BLUE 0xAA0000UL
#define GREEN 0x00AA00UL
#define RED 0x0000AAUL
#define YELLOW 0x55FFFFUL
#define WHITE 0xFFFFFFUL
#define LIGHTGRAY 0xAAAAAAUL
enum { TRANSPARENT = 1, OPAQUE = 2 };
enum { PS_SOLID = 0 };
enum { VK_LEFT = 0x25, VK_UP, VK_RIGHT, VK_DOWN, VK_SPACE = 0x20, VK_ESCAPE = 0x1B };
#define srand(x) srand(emu_seed())
inline unsigned emu_seed() { const char* s = getenv("SEED"); return s ? atoi(s) : 1; }

namespace emu {
inline int W = 640, H = 480, frame = 0, getchCount = 0;
inline COLORREF bk = BLACK, lc = WHITE, fc = WHITE, tc = WHITE;
inline int lw = 1, bkmode = OPAQUE, th = 16;
inline std::vector<std::string> els;
inline std::string col(COLORREF c) { char b[16]; snprintf(b, 16, "#%02x%02x%02x", (int)(c & 255), (int)((c >> 8) & 255), (int)((c >> 16) & 255)); return b; }
inline void add(const char* fmt, ...) { char b[1024]; va_list a; va_start(a, fmt); vsnprintf(b, sizeof b, fmt, a); va_end(a); els.push_back(b); }
inline std::string stroke() { char b[96]; snprintf(b, 96, "stroke=\"%s\" stroke-width=\"%d\"", col(lc).c_str(), lw); return b; }
inline void snap() {
    const char* out = getenv("OUT"); FILE* f = fopen(out ? out : "out.svg", "w");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\" shape-rendering=\"crispEdges\">\n", W, H, W, H);
    for (auto& e : els) fprintf(f, "%s\n", e.c_str());
    fprintf(f, "</svg>\n"); fclose(f); exit(0);
}
// Key script: KEYS="SPACE:40-42,LEFT:10-30"  (held during those frame numbers)
inline bool held(int vk) {
    const char* k = getenv("KEYS"); if (!k) return false;
    std::string s(k); size_t p = 0;
    while (p < s.size()) {
        size_t q = s.find(',', p); if (q == std::string::npos) q = s.size();
        std::string it = s.substr(p, q - p); p = q + 1;
        char name[16]; int a, b; if (sscanf(it.c_str(), "%15[^:]:%d-%d", name, &a, &b) != 3) continue;
        int code = !strcmp(name, "SPACE") ? VK_SPACE : !strcmp(name, "LEFT") ? VK_LEFT : !strcmp(name, "RIGHT") ? VK_RIGHT :
                   !strcmp(name, "UP") ? VK_UP : !strcmp(name, "DOWN") ? VK_DOWN : -1;
        if (code == vk && frame >= a && frame <= b) return true;
    }
    return false;
}
inline void tick() { frame++; if (frame > 5000) snap(); const char* s = getenv("SNAP"); if (s && frame == atoi(s)) snap(); }
}
inline void initgraph(int w, int h, int = 0) { emu::W = w; emu::H = h; emu::els.clear(); emu::add("<rect width=\"%d\" height=\"%d\" fill=\"#000000\"/>", w, h); }
inline void closegraph() {}
inline void setbkcolor(COLORREF c) { emu::bk = c; }
inline void cleardevice() { emu::els.clear(); emu::add("<rect width=\"%d\" height=\"%d\" fill=\"%s\"/>", emu::W, emu::H, emu::col(emu::bk).c_str()); }
inline void setlinecolor(COLORREF c) { emu::lc = c; }
inline void setfillcolor(COLORREF c) { emu::fc = c; }
inline void settextcolor(COLORREF c) { emu::tc = c; }
inline void setbkmode(int m) { emu::bkmode = m; }
inline void setlinestyle(int, int w = 1) { emu::lw = w; }
inline void settextstyle(int h, int, LPCTSTR) { emu::th = h; }
inline void line(int a, int b, int c, int d) { emu::add("<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" %s stroke-linecap=\"round\"/>", a, b, c, d, emu::stroke().c_str()); }
inline void circle(int x, int y, int r) { emu::add("<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"none\" %s/>", x, y, r, emu::stroke().c_str()); }
inline void fillcircle(int x, int y, int r) { emu::add("<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"%s\" %s/>", x, y, r, emu::col(emu::fc).c_str(), emu::stroke().c_str()); }
inline void solidcircle(int x, int y, int r) { emu::add("<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"%s\"/>", x, y, r, emu::col(emu::fc).c_str()); }
inline void rectangle(int l, int t, int r, int b) { emu::add("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"none\" %s/>", l, t, r - l, b - t, emu::stroke().c_str()); }
inline void fillrectangle(int l, int t, int r, int b) { emu::add("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"%s\" %s/>", l, t, r - l, b - t, emu::col(emu::fc).c_str(), emu::stroke().c_str()); }
inline void solidrectangle(int l, int t, int r, int b) { emu::add("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"%s\"/>", l, t, r - l, b - t, emu::col(emu::fc).c_str()); }
inline void fillpolygon(const POINT* p, int n) {
    std::string pts; char b[32];
    for (int i = 0; i < n; i++) { snprintf(b, 32, "%ld,%ld ", p[i].x, p[i].y); pts += b; }
    emu::add("<polygon points=\"%s\" fill=\"%s\" %s/>", pts.c_str(), emu::col(emu::fc).c_str(), emu::stroke().c_str());
}
// EasyX arc: ellipse bounding box, angles in RADIANS, counter-clockwise with y up
inline void arc(int l, int t, int r, int b, double s, double e) {
    double cx = (l + r) / 2.0, cy = (t + b) / 2.0, rx = (r - l) / 2.0, ry = (b - t) / 2.0, tp = 2 * M_PI;
    s = fmod(fmod(s, tp) + tp, tp); e = fmod(fmod(e, tp) + tp, tp); double sweep = fmod(e - s + tp, tp);
    emu::add("<path d=\"M %.1f %.1f A %.1f %.1f 0 %d 0 %.1f %.1f\" fill=\"none\" %s/>", cx + rx * cos(s), cy - ry * sin(s), rx, ry,
             sweep > M_PI ? 1 : 0, cx + rx * cos(e), cy - ry * sin(e), emu::stroke().c_str());
}
inline void outtextxy(int x, int y, LPCTSTR s) {
    std::string t; for (const char* p = s; *p; p++) { if (*p == '<') t += "&lt;"; else if (*p == '&') t += "&amp;"; else t += *p; }
    if (emu::bkmode == OPAQUE) emu::add("<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"%s\"/>", x, y, (int)(strlen(s) * emu::th * 0.55), emu::th, emu::col(emu::bk).c_str());
    emu::add("<text x=\"%d\" y=\"%d\" font-family=\"Consolas, 'DejaVu Sans Mono', monospace\" font-size=\"%d\" fill=\"%s\" dominant-baseline=\"text-before-edge\" textLength=\"%d\" lengthAdjust=\"spacingAndGlyphs\">%s</text>", x, y, emu::th, emu::col(emu::tc).c_str(), (int)(strlen(s) * emu::th * 0.55), t.c_str());
}
inline void BeginBatchDraw() {}
inline void FlushBatchDraw() { emu::tick(); }
inline void EndBatchDraw() {}
inline void Sleep(int) {}
inline void Beep(int, int) {}
inline short GetAsyncKeyState(int vk) { return emu::held(vk) ? (short)0x8000 : 0; }
inline int _kbhit() { int k[] = {VK_SPACE, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN}; for (int v : k) if (emu::held(v)) return 1; return 0; }
inline int _getch() { emu::getchCount++; const char* s = getenv("GETCH_SNAP"); if (s && emu::getchCount == atoi(s)) emu::snap(); return 0; }
#define _stprintf sprintf
#define _stprintf_s sprintf
