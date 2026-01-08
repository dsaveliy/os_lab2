#define NOMINMAX
#include "winapithread.h"
#include "matrix.h"
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <mutex>

std::mutex result_mutex;

struct WinBlockParams {
    const std::vector<std::vector<int>>* a;
    const std::vector<std::vector<int>>* b;
    std::vector<std::vector<int>>* result;
    int rowBlock;
    int colBlock;
    int innerBlock;
    int blockSize;
    int n;
};

DWORD WINAPI BlockWorker(LPVOID lpParam) {
    WinBlockParams* p = static_cast<WinBlockParams*>(lpParam);

    int rowStartA = p->rowBlock * p->blockSize;
    int rowEndA = std::min(rowStartA + p->blockSize, p->n);
    int colStartA = p->innerBlock * p->blockSize;
    int colEndA = std::min(colStartA + p->blockSize, p->n);

    int rowStartB = p->innerBlock * p->blockSize;
    int rowEndB = std::min(rowStartB + p->blockSize, p->n);
    int colStartB = p->colBlock * p->blockSize;
    int colEndB = std::min(colStartB + p->blockSize, p->n);

    int rowLen = rowEndA - rowStartA;
    int colLen = colEndB - colStartB;
    int innerLen = std::min(colEndA - colStartA, rowEndB - rowStartB);

    for (int i = 0; i < rowLen; ++i) {
        int globalRow = rowStartA + i;
        for (int j = 0; j < colLen; ++j) {
            int globalCol = colStartB + j;
            int sum = 0;

            for (int k = 0; k < innerLen; ++k) {
                int globalK = colStartA + k;
                sum += (*p->a)[globalRow][globalK] * (*p->b)[globalK][globalCol];
            }

            std::lock_guard<std::mutex> lock(result_mutex);
            (*p->result)[globalRow][globalCol] += sum;
        }
    }

    delete p;
    return 0;
}

std::vector<std::vector<int>> multiplyMatricesByBlocksWinAPI(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b,
    int k
) {
    int n = a.size();
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));

    int numBlocks = n / k + (n % k != 0 ? 1 : 0);
    std::vector<HANDLE> handles;

    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock) {
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            for (int innerBlock = 0; innerBlock < numBlocks; ++innerBlock) {
                WinBlockParams* param = new WinBlockParams{
                    &a, &b, &result,
                    rowBlock, colBlock, innerBlock,
                    k, n
                };

                HANDLE hThread = CreateThread(nullptr, 0, BlockWorker, param, 0, nullptr);
                if (hThread) {
                    handles.push_back(hThread);
                }
                else {
                    delete param;
                }
            }
        }
    }

    WaitForMultipleObjects(handles.size(), handles.data(), TRUE, INFINITE);

    for (HANDLE h : handles) {
        CloseHandle(h);
    }

    return result;
}