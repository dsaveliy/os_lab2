#define NOMINMAX // для std::min/max
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
    // распаковывем параметры
    int startRowA = p->rowBlock * p->blockSize;
    int endRowA = std::min(startRowA + p->blockSize, p->n);
    int startColA = p->innerBlock * p->blockSize;
    int endColA = std::min(startColA + p->blockSize, p->n);
    
    int startRowB = p->innerBlock * p->blockSize;
    int endRowB = std::min(startRowB + p->blockSize, p->n);
    int startColB = p->colBlock * p->blockSize;
    int endColB = std::min(startColB + p->blockSize, p->n);

    for (int i = startRowA; i < endRowA; ++i) {
        for (int j = startColB; j < endColB; ++j) {
            int sum = 0;
            for (int k = 0; k < p->blockSize; ++k) {
                int colA = startColA + k;
                int rowB = startRowB + k;
                
                if (colA < endColA && rowB < endRowB) {
                    sum += (*p->a)[i][colA] * (*p->b)[rowB][j];
                }
            }
            
            std::lock_guard<std::mutex> lock(result_mutex);
            (*p->result)[i][j] += sum;
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
                } else {
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


