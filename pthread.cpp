#include "pthread.h"
#include "matrix.h"
#include <pthread.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

struct WorkerArgs {
    const std::vector<std::vector<int>>* a;
    const std::vector<std::vector<int>>* b;
    std::vector<std::vector<int>>* result;
    int rowBlock;
    int colBlock;
    int k;
    int n;
    int numBlocks;
};

void* block_worker(void* p) {
    WorkerArgs* args = static_cast<WorkerArgs*>(p);
    const std::vector<std::vector<int>>& a = *args->a;
    const std::vector<std::vector<int>>& b = *args->b;
    std::vector<std::vector<int>>& result = *args->result;
    int rowBlock = args->rowBlock;
    int colBlock = args->colBlock;
    int k = args->k;
    int n = args->n;
    int numBlocks = args->numBlocks;

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

    return nullptr;
}

std::vector<std::vector<int>> multiplyMatricesByBlocksPthread(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int k
) {
    int n = (int)a.size();
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));
    int numBlocks = n / k + ((n % k) ? 1 : 0);

    std::vector<pthread_t> threadIds;
    std::vector<WorkerArgs> workerArgs;
    threadIds.reserve(numBlocks * numBlocks);
    workerArgs.reserve(numBlocks * numBlocks);

    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock) {
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            workerArgs.push_back({ &a, &b, &result, rowBlock, colBlock, k, n, numBlocks });
            threadIds.push_back(0);
            int tidx = threadIds.size() - 1;
            int rc = pthread_create(&threadIds[tidx], nullptr, &block_worker, (void*)&workerArgs.back());
            if (rc != 0) {
                block_worker((void*)&workerArgs.back());
                threadIds[tidx] = 0;
            }
        }
    }

    for (size_t i = 0; i < threadIds.size(); ++i) {
        if (threadIds[i] != 0) pthread_join(threadIds[i], nullptr);
    }
    return result;
}

