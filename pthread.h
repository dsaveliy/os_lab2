#pragma once
#include <vector>

void* block_worker(void* p);

std::vector<std::vector<int>> multiplyMatricesByBlocksPthread(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b, int k, int max_threads = 0);