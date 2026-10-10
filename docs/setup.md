# 0. Setup: C++ + EasyX on Windows

EasyX is a small graphics library for C/C++ on **Windows**. It lets you open a window and draw shapes, text and images with a few function calls, which makes it perfect for learning game programming. The code in this repository will not run on macOS or Linux (except through a Windows virtual machine).

There are two ways to set things up. Pick **one**.

| | Option A: VS Code + MinGW | Option B: Visual Studio 2022 |
|---|---|---|
| Size | Small | Large |
| Good for | Lightweight editing, the course's VS Code tutorial | The course lectures' default IDE |
| Build | `build.bat` or Ctrl+Shift+B | F5 |

---

## Option A: VS Code + MinGW-w64 + EasyX

### 1. Install VS Code

Download it from <https://code.visualstudio.com/> and run the installer.

### 2. Install the MinGW-w64 compiler (gcc / g++)

1. Download MinGW-w64 from <https://www.mingw-w64.org/> (or use the `mingw-w64-8.1.0` package from your course materials).
2. Unzip it to a path **without spaces or non-English characters**, for example `C:\mingw64`.
3. Add the `bin` folder to your `Path`:
   *Start → "Edit the system environment variables" → Environment Variables → System variables → Path → Edit → New →* `C:\mingw64\bin` → OK, OK, OK.
4. Open a **new** Command Prompt and check:

   ```bat
   gcc --version
   ```

   If you see `'gcc' is not recognized…`, the `Path` entry is wrong. Double-check the folder name.

### 3. Install EasyX for MinGW

1. Download the MinGW version of EasyX from <https://easyx.cn/> (or use `easyx4mingw_25.9.10` from your course materials) and unzip it.
2. Copy everything in EasyX's `include` folder into `C:\mingw64\include` (actually `C:\mingw64\x86_64-w64-mingw32\include` on some MinGW builds; use the folder that already contains `stdio.h`).
3. Copy everything in EasyX's `lib64` folder into `C:\mingw64\lib` (or `C:\mingw64\x86_64-w64-mingw32\lib`, next to `libgdi32.a`).

### 4. Configure VS Code

1. Install the **C/C++** extension by Microsoft from the Extensions panel.
2. Open this repository folder in VS Code (*File → Open Folder*).
3. The `.vscode` folder in this repo already has a build task. If your MinGW is not in `C:\mingw64`, edit `compilerPath` in `.vscode/c_cpp_properties.json`.

### 5. Build and run

From the repository folder in a terminal:

```bat
build.bat lessons\01-drawing-on-screen\hello_shapes.cpp
```

Or open any lesson `.cpp` file and press **Ctrl+Shift+B** (*Build & run current EasyX file*).

The command `build.bat` runs is:

```bat
g++ file.cpp -o file.exe -leasyx -lgdi32 -lole32 -luuid
```

The four `-l` flags link EasyX and the Windows libraries it needs. Forgetting them is the #1 setup error (see below).

---

## Option B: Visual Studio 2022 + EasyX

1. Install **Visual Studio 2022 Community** with the *Desktop development with C++* workload.
2. Install EasyX from <https://easyx.cn/>: its installer detects Visual Studio and adds itself automatically.
3. Create an *Empty Project*, add a new `.cpp` file, paste a lesson's code in, and press **F5**.
4. Set **Project → Properties → Advanced → Character Set → Use Multi-Byte Character Set**. The lessons pass plain `"text"` strings to `outtextxy`, which needs this setting in Visual Studio.

---

## 🩺 Troubleshooting

| Message | Cause | Fix |
|---|---|---|
| `graphics.h: No such file or directory` | EasyX headers not copied | Redo step 3.2 into the folder that has `stdio.h` |
| `undefined reference to initgraph` | EasyX library not linked | Add `-leasyx -lgdi32 -lole32 -luuid`, or check step 3.3 |
| `'gcc' is not recognized` | `Path` not set, or old terminal | Fix `Path`, then open a new terminal |
| `cannot convert 'const char*' to 'LPCTSTR'` (Visual Studio) | Project uses Unicode | Use Multi-Byte Character Set (Option B, step 4), or wrap strings in `_T("...")` |
| Window opens and closes instantly | Program reached `closegraph()` | Make sure there is a `_getch()` or a game loop before it |
| Screen flickers | Drawing directly to the window | Use `BeginBatchDraw()` / `FlushBatchDraw()` (see [Lesson 2](../lessons/02-game-loop-runner/)) |
