@echo off

set COMPILER_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

call %COMPILER_PATH% x64

cl.exe /EHsc /openmp Lab_6_1.cpp /Fe:Lab_6_1.exe

if %errorlevel% equ 0 (
    echo good
    Lab_6_1.exe
) else (
    echo no good
)