#define NOMINMAX // для std::min/max
#include "winapithread.h"
#include "matrix.h"
#include <windows.h>
#include <algorithm>
#include <iostream>

struct WinBlockParams {
    const std::vector<std::vector<int>>* a;
    const std::vector<std::vector<int>>* b;
    std::vector<std::vector<int>>* result;
    int rowBlock, colBlock, k, n, numBlocks;
};

DWORD WINAPI BlockWorker(LPVOID lpParam) {
    WinBlockParams* p = static_cast<WinBlockParams*>(lpParam);
    // распаковывем параметры
    const auto& a = *(p->a);
    const auto& b = *(p->b);
    auto& result = *(p->result);

    int rowStart = p->rowBlock * p->k, colStart = p->colBlock * p->k;
    int rowLen = std::min(p->k, p->n - rowStart), colLen = std::min(p->k, p->n - colStart);

    std::vector<std::vector<int>> block(rowLen, std::vector<int>(colLen, 0));

    for (int innerBlock = 0; innerBlock < p->numBlocks; ++innerBlock) {
        int innerStart = innerBlock * p->k;
        int midLen = std::min(p->k, p->n - innerStart);
        std::vector<std::vector<int>> aBlock(rowLen, std::vector<int>(midLen));
        std::vector<std::vector<int>> bBlock(midLen, std::vector<int>(colLen));
        for (int i = 0; i < rowLen; ++i)
            for (int m = 0; m < midLen; ++m) {
                aBlock[i][m] = a[rowStart + i][innerStart + m];
            }
        for (int m = 0; m < midLen; ++m)
            for (int j = 0; j < colLen; ++j) {
                bBlock[m][j] = b[innerStart + m][colStart + j];
            }
        auto blockResult = multiplyMatrices(aBlock, bBlock);
        for (int i = 0; i < rowLen; ++i)
            for (int j = 0; j < colLen; ++j)
                block[i][j] += blockResult[i][j];
    }

    for (int i = 0; i < rowLen; ++i)
        for (int j = 0; j < colLen; ++j) {
            result[rowStart + i][colStart + j] = block[i][j];
        }

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
    std::vector<WinBlockParams> params;
    params.reserve(numBlocks * numBlocks); // без резервирования могут слетать память и биться значения
    std::vector<HANDLE> handles;

    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock)
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            // упаковывем параметры в один обьект для передачи по ссылке
            WinBlockParams param;
            param.a = &a;
            param.b = &b;
            param.result = &result;
            param.rowBlock = rowBlock;
            param.colBlock = colBlock;
            param.k = k;
            param.n = n;
            param.numBlocks = numBlocks;
            params.push_back(param);
            handles.push_back(CreateThread(nullptr, 0, BlockWorker, &params.back(), 0, nullptr));
        }

    WaitForMultipleObjects(handles.size(), handles.data(), TRUE, INFINITE);
    for (HANDLE h : handles) CloseHandle(h);

    return result;
}
