#include <windows.h>
#include <vector>

// функция для блока gтока winapi
DWORD WINAPI BlockWorker(LPVOID lpParam);

// распределение блоков умножения по потокам и создание обьектов параметров
std::vector<std::vector<int>> multiplyMatricesByBlocksWinAPI(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int k
);