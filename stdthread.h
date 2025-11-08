#pragma once
#include <vector>

// обработка конкретного блока в одном потоке
void blockWorker(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& result, int rowBlock, int colBlock, int k, int n, int numBlocks);

// распределение блоков умножения по потокам
std::vector<std::vector<int>> multiplyMatricesByBlocksThread(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b, int k);