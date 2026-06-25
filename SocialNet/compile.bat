@echo off
REM Compile and run SocialNet project for Windows

echo Compiling SocialNet...
g++ -std=c++17 -O2 -o SocialNet.exe src\main.cpp

IF %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Running testcases from sample_input.txt...
    SocialNet.exe < sample_input.txt
) ELSE (
    echo Compilation failed. Check errors above.
)

pause
