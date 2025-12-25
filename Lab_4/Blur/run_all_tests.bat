@echo off
setlocal enabledelayedexpansion

echo ==================================================
echo     LAB WORK 4 - AUTOMATED TESTING SCRIPT
echo ==================================================
echo.

if not exist "test.bmp" (
    echo ERROR: test.bmp not found!
    echo Please create a test BMP image first.
    pause
    exit /b 1
)

if not exist "Blur.exe" (
    echo ERROR: Blur.exe not found!
    echo Please compile the program first.
    pause
    exit /b 1
)

echo Starting Lab Work 4 tests...
echo.

REM Создаем папку для результатов
if not exist "lab4_results" mkdir lab4_results
if not exist "lab4_results\logs" mkdir lab4_results\logs
if not exist "lab4_results\outputs" mkdir lab4_results\outputs

echo Testing configurations from the assignment...
echo.

REM ==================================================
REM Тест A: 1 процессор, 3 потока, одинаковые приоритеты
echo ===== TEST A: 1 CORE, 3 THREADS, SAME PRIORITY =====
Blur.exe test.bmp lab4_results\outputs\test_a.bmp --threads 3 --cores 1 --priority NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_a.csv
echo.

REM ==================================================
REM Тест B: 1 процессор, 3 потока, разные приоритеты (вариант 1)
echo ===== TEST B: 1 CORE, 3 THREADS, MIXED PRIORITY 1 =====
Blur.exe test.bmp lab4_results\outputs\test_b.bmp --threads 3 --cores 1 --priority ABOVE_NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_b.csv
echo.

REM ==================================================
REM Тест C: 1 процессор, 3 потока, разные приоритеты (вариант 2)
echo ===== TEST C: 1 CORE, 3 THREADS, MIXED PRIORITY 2 =====
Blur.exe test.bmp lab4_results\outputs\test_c.bmp --threads 3 --cores 1 --priority ABOVE_NORMAL NORMAL BELOW_NORMAL --iterations 1 --logfile lab4_results\logs\test_c.csv
echo.

REM ==================================================
REM Тест D: 2 процессора, 3 потока, одинаковые приоритеты
echo ===== TEST D: 2 CORES, 3 THREADS, SAME PRIORITY =====
Blur.exe test.bmp lab4_results\outputs\test_d.bmp --threads 3 --cores 2 --priority NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_d.csv
echo.

REM ==================================================
REM Тест E: 2 процессора, 3 потока, разные приоритеты (вариант 1)
echo ===== TEST E: 2 CORES, 3 THREADS, MIXED PRIORITY 1 =====
Blur.exe test.bmp lab4_results\outputs\test_e.bmp --threads 3 --cores 2 --priority ABOVE_NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_e.csv
echo.

REM ==================================================
REM Тест F: 2 процессора, 3 потока, разные приоритеты (вариант 2)
echo ===== TEST F: 2 CORES, 3 THREADS, MIXED PRIORITY 2 =====
Blur.exe test.bmp lab4_results\outputs\test_f.bmp --threads 3 --cores 2 --priority ABOVE_NORMAL NORMAL BELOW_NORMAL --iterations 1 --logfile lab4_results\logs\test_f.csv
echo.

REM ==================================================
REM Тест G: 3 процессора, 3 потока, одинаковые приоритеты
echo ===== TEST G: 3 CORES, 3 THREADS, SAME PRIORITY =====
Blur.exe test.bmp lab4_results\outputs\test_g.bmp --threads 3 --cores 3 --priority NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_g.csv
echo.

REM ==================================================
REM Тест H: 3 процессора, 3 потока, разные приоритеты (вариант 1)
echo ===== TEST H: 3 CORES, 3 THREADS, MIXED PRIORITY 1 =====
Blur.exe test.bmp lab4_results\outputs\test_h.bmp --threads 3 --cores 3 --priority ABOVE_NORMAL NORMAL NORMAL --iterations 1 --logfile lab4_results\logs\test_h.csv
echo.

REM ==================================================
REM Тест I: 3 процессора, 3 потока, разные приоритеты (вариант 2)
echo ===== TEST I: 3 CORES, 3 THREADS, MIXED PRIORITY 2 =====
Blur.exe test.bmp lab4_results\outputs\test_i.bmp --threads 3 --cores 3 --priority ABOVE_NORMAL NORMAL BELOW_NORMAL --iterations 1 --logfile lab4_results\logs\test_i.csv
echo.

REM ==================================================
REM Тест J: Последовательная обработка для сравнения
echo ===== TEST J: SEQUENTIAL PROCESSING =====
Blur.exe test.bmp lab4_results\outputs\test_j.bmp --sequential --iterations 1 --logfile lab4_results\logs\test_j.csv
echo.

echo ==================================================
echo All tests completed!
echo.
echo Results summary:
echo.

REM Создаем файл с описанием тестов
echo Test Configuration Summary > lab4_results\test_summary.txt
echo ========================= >> lab4_results\test_summary.txt
echo. >> lab4_results\test_summary.txt

echo TEST A: 1 Core, 3 Threads, Priority: NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST B: 1 Core, 3 Threads, Priority: ABOVE_NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST C: 1 Core, 3 Threads, Priority: ABOVE_NORMAL NORMAL BELOW_NORMAL >> lab4_results\test_summary.txt
echo TEST D: 2 Cores, 3 Threads, Priority: NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST E: 2 Cores, 3 Threads, Priority: ABOVE_NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST F: 2 Cores, 3 Threads, Priority: ABOVE_NORMAL NORMAL BELOW_NORMAL >> lab4_results\test_summary.txt
echo TEST G: 3 Cores, 3 Threads, Priority: NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST H: 3 Cores, 3 Threads, Priority: ABOVE_NORMAL NORMAL NORMAL >> lab4_results\test_summary.txt
echo TEST I: 3 Cores, 3 Threads, Priority: ABOVE_NORMAL NORMAL BELOW_NORMAL >> lab4_results\test_summary.txt
echo TEST J: Sequential Processing >> lab4_results\test_summary.txt

type lab4_results\test_summary.txt
echo.
echo Log files are in: lab4_results\logs\
echo Output images are in: lab4_results\outputs\
echo.
echo ==================================================
pause