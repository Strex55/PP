#include "stdafx.h"
#include <windows.h>
#include <string>
#include <iostream>

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    int threadNumber = *(int*)lpParam;
    std::cout << "ѕоток є" << threadNumber << " выполн€ет свою работу" << std::endl;
    ExitThread(0);
}

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    if (argc != 2) {
        std::cout << "»спользование: " << argv[0] << " <количество_потоков>" << std::endl;
        return 1;
    }

    int N = atoi(argv[1]);

    if (N <= 0) {
        std::cout << " оличество потоков должно быть положительным числом!" << std::endl;
        return 1;
    }

    // создание N потоков
    HANDLE* handles = new HANDLE[N];
    int* threadNumbers = new int[N];

    for (int i = 0; i < N; i++) {
        threadNumbers[i] = i + 1;
        handles[i] = CreateThread(NULL, 0, &ThreadProc, &threadNumbers[i], CREATE_SUSPENDED, NULL);
    }

    // запуск N потоков
    for (int i = 0; i < N; i++) {
        ResumeThread(handles[i]);
    }

    // ожидание окончани€ работы всех потоков
    WaitForMultipleObjects(N, handles, true, INFINITE);

    // закрытие дескрипторов
    for (int i = 0; i < N; i++) {
        CloseHandle(handles[i]);
    }

    delete[] handles;
    delete[] threadNumbers;

    return 0;
}