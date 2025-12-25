@echo off
setlocal enabledelayedexpansion

echo Running performance tests...

REM Test different configurations
for /l %%c in (1,1,4) do (
    for /l %%t in (1,1,16) do (
        echo Testing: Cores=%%c Threads=%%t
        Blur.exe input.bmp output_%%c_%%t.bmp %%t %%c parallel
    )
)

REM Sequential test
echo Testing: Sequential
Blur.exe input.bmp output.bmp 1 1 sequential

echo All tests completed!
pause