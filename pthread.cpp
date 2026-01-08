#include "pthread.h"
#include "matrix.h"
#include <pthread.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <mutex>

std::mutex result_mutex_pthread;

void* block_worker(void* p) {
    WorkerArgs* args = static_cast<WorkerArgs*>(p);

    int rowStartA = args->rowBlock * args->k;
    int rowEndA = std::min(rowStartA + args->k, args->n);
    int colStartA = args->innerBlock * args->k;
    int colEndA = std::min(colStartA + args->k, args->n);

    int rowStartB = args->innerBlock * args->k;
    int rowEndB = std::min(rowStartB + args->k, args->n);
    int colStartB = args->colBlock * args->k;
    int colEndB = std::min(colStartB + args->k, args->n);

    int rowLen = rowEndA - rowStartA;
    int colLen = colEndB - colStartB;
    int innerLen = std::min(args->k, args->n - args->innerBlock * args->k);

    for (int i = 0; i < rowLen; ++i) {
        int globalRow = rowStartA + i;
        for (int j = 0; j < colLen; ++j) {
            int globalCol = colStartB + j;
            int sum = 0;

            for (int k = 0; k < innerLen; ++k) {
                int globalK = colStartA + k;
                sum += (*args->a)[globalRow][globalK] * (*args->b)[globalK][globalCol];
            }

            std::lock_guard<std::mutex> lock(result_mutex_pthread);
            (*args->result)[globalRow][globalCol] += sum;
        }
    }

    delete args;
    return nullptr;
}

std::vector<std::vector<int>> multiplyMatricesByBlocksPthread(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int k
) {
    int n = static_cast<int>(a.size());
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));

    int numBlocks = n / k + (n % k != 0 ? 1 : 0);
    std::vector<pthread_t> threads;

    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock) {
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            for (int innerBlock = 0; innerBlock < numBlocks; ++innerBlock) {
                WorkerArgs* args = new WorkerArgs{
                    &a, &b, &result,
                    rowBlock, colBlock, innerBlock,
                    k, n
                };

                pthread_t thread;
                if (pthread_create(&thread, nullptr, &block_worker, args) == 0) {
                    threads.push_back(thread);
                }
                else {
                    delete args;
                }
            }
        }
    }

    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    return result;
}