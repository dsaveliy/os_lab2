#pragma once
#include <vector>
#include <mutex>

// Обработка одного блока в одном потоке (для одного innerBlock)
void blockWorker(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& result,
    int rowBlock, int colBlock, int innerBlock,
    int blockSize, int n);

// Распределение блоков умножения по потокам
std::vector<std::vector<int>> multiplyMatricesByBlocksThread(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int blockSize);