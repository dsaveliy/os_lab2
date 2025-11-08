#include "matrix.h"
#include "stdthread.h"
#include <thread>

std::vector<std::vector<int>> multiplyMatricesByBlocksThread(const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b, int k) {
    int n = a.size();
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));
    int numBlocks = n / k + (n % k != 0 ? 1 : 0);

    // фактические разбил ту же логику multiplyMatricesByBlocks на потоки
    std::vector<std::thread> threads;
    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock)
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock)
            threads.emplace_back(blockWorker, std::cref(a), std::cref(b), std::ref(result),
                rowBlock, colBlock, k, n, numBlocks);
    for (auto& t : threads) 
        t.join();
    return result;
}

void blockWorker(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b,
    std::vector<std::vector<int>>& result, int rowBlock, int colBlock, int k, int n, int numBlocks) {
    int rowStart = rowBlock * k;
    int colStart = colBlock * k;
    int rowLen = std::min(k, n - rowStart);
    int colLen = std::min(k, n - colStart);
    std::vector<std::vector<int>> block(rowLen, std::vector<int>(colLen, 0));

    for (int innerBlock = 0; innerBlock < numBlocks; ++innerBlock) {
        int innerStart = innerBlock * k;
        int midLen = std::min(k, n - innerStart);
        std::vector<std::vector<int>> aBlock(rowLen, std::vector<int>(midLen));
        std::vector<std::vector<int>> bBlock(midLen, std::vector<int>(colLen));
        for (int i = 0; i < rowLen; ++i)
            for (int m = 0; m < midLen; ++m)
                aBlock[i][m] = a[rowStart + i][innerStart + m];
        for (int m = 0; m < midLen; ++m)
            for (int j = 0; j < colLen; ++j)
                bBlock[m][j] = b[innerStart + m][colStart + j];
        std::vector<std::vector<int>> blockResult = multiplyMatrices(aBlock, bBlock);
        for (int i = 0; i < rowLen; ++i)
            for (int j = 0; j < colLen; ++j)
                block[i][j] += blockResult[i][j];
    }
    for (int i = 0; i < rowLen; ++i)
        for (int j = 0; j < colLen; ++j)
            result[rowStart + i][colStart + j] = block[i][j];
}
