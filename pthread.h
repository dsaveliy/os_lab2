#pragma o#pragma once
#include <vector>

struct WorkerArgs {
    const std::vector<std::vector<int>>* a;
    const std::vector<std::vector<int>>* b;
    std::vector<std::vector<int>>* result;
    int rowBlock;
    int colBlock;
    int innerBlock;
    int k;
    int n;
};

void* block_worker(void* p);
std::vector<std::vector<int>> multiplyMatricesByBlocksPthread(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int k
);