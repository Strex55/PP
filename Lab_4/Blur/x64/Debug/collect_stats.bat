@echo off
setlocal enabledelayedexpansion

echo Performance Test Script
echo ======================

REM Создаем файл результатов
echo Cores,Threads,Time_ms > results.csv

echo Testing all configurations...
echo.

for /l %%c in (1,1,4) do (
    for /l %%t in (1,1,16) do (
        echo Testing: Cores=%%c, Threads=%%t
        
        REM Запускаем программу и записываем вывод в файл
        Blur.exe test.bmp temp.bmp %%t %%c parallel > output.txt 2>&1
        
        REM Ищем строку с временем
        set "time_value=0"
        for /f "usebackq tokens=2 delims= " %%a in (`findstr "Time:" output.txt`) do (
            set "time_value=%%a"
        )
        
        echo Time: !time_value! ms
        echo %%c,%%t,!time_value! >> results.csv
        
        REM Очистка
        del output.txt 2>nul
        if exist temp.bmp del temp.bmp 2>nul
    )
)

echo.
echo Results saved to results.csv
echo.
echo Contents of results.csv:
echo ========================
type results.csv
echo ========================
echo.
pause