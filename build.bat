@echo off
rem build.bat -- compile and run one EasyX lesson file with MinGW g++
rem Usage: build.bat lessons\01-drawing-on-screen\hello_shapes.cpp

if "%~1"=="" (
    echo Usage: build.bat path\to\file.cpp
    exit /b 1
)

set "SRC=%~1"
set "EXE=%~dpn1.exe"

echo Building %SRC% ...
g++ -std=gnu++11 "%SRC%" -o "%EXE%" -leasyx -lgdi32 -lole32 -luuid
if errorlevel 1 (
    echo.
    echo Build failed. See docs\setup.md "Troubleshooting".
    exit /b 1
)

echo Running %EXE% ...
pushd "%~dp1"
"%EXE%"
popd
