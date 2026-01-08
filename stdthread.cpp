#include "stdthread.h"
#include <thread>
#include <algorithm>
#include <vector>
#include <mutex>

std::mutex result_mutex;

void blockWorker(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& result,
    int rowBlock, int colBlock, int innerBlock,
    int blockSize, int n) {

    int rowStartA = rowBlock * blockSize;
    int rowEndA = std::min(rowStartA + blockSize, n);
    int colStartA = innerBlock * blockSize;
    int colEndA = std::min(colStartA + blockSize, n);

    int rowStartB = innerBlock * blockSize;
    int rowEndB = std::min(rowStartB + blockSize, n);
    int colStartB = colBlock * blockSize;
    int colEndB = std::min(colStartB + blockSize, n);

    int rowLen = rowEndA - rowStartA;
    int colLen = colEndB - colStartB;
    int innerLen = colEndA - colStartA;

    for (int i = 0; i < rowLen; ++i) {
        int globalRow = rowStartA + i;
        for (int j = 0; j < colLen; ++j) {
            int globalCol = colStartB + j;
            int sum = 0;
            for (int k = 0; k < innerLen; ++k) {
                int globalK = colStartA + k;
                sum += a[globalRow][globalK] * b[globalK][globalCol];
            }
            std::lock_guard<std::mutex> lock(result_mutex);
            result[globalRow][globalCol] += sum;
        }
    }
}

std::vector<std::vector<int>> multiplyMatricesByBlocksThread(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int blockSize) {

    int n = a.size();
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));

    int numBlocks = n / blockSize + (n % blockSize != 0 ? 1 : 0);
    std::vector<std::thread> threads;
    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock) {
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            for (int innerBlock = 0; innerBlock < numBlocks; ++innerBlock) {
                threads.emplace_back(blockWorker,
                    std::cref(a), std::cref(b), std::ref(result),
                    rowBlock, colBlock, innerBlock,
                    blockSize, n);
            }
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return result;
}