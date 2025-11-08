#include "matrix.h"
#include <cstdlib>

std::vector<std::vector<int>> generateMatrix(int N) {
    srand(18);
    std::vector<std::vector<int>> m(N, std::vector<int>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            m[i][j] = rand() % 100;
        }
    }
    return m;
}

std::vector<std::vector<int>> readMatrixFromFile(const std::string& filename) {
    std::ifstream fin(filename);
    int n;
    fin >> n;
    std::vector<std::vector<int>> matrix(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            fin >> matrix[i][j];
    return matrix;
}

std::vector<std::vector<int>> multiplyMatrices(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b) {
    int m = a.size();
    int n = a[0].size();
    int p = b[0].size();
    std::vector<std::vector<int>> result(m, std::vector<int>(p, 0));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < p; ++j)
            for (int k = 0; k < n; ++k)
                result[i][j] += a[i][k] * b[k][j];
    return result;
}

std::vector<std::vector<int>> multiplyMatricesByBlocks(const std::vector<std::vector<int>>& a, const std::vector<std::vector<int>>& b, int k) {
    int n = a.size();
    // учитывая, что матрицы берём квадратные
    std::vector<std::vector<int>> result(n, std::vector<int>(n, 0));
    int numBlocks = n / k + (n % k != 0 ? 1 : 0);

    //рассмотрим каждыфй блок итоговой матрицы
    for (int rowBlock = 0; rowBlock < numBlocks; ++rowBlock) {
        for (int colBlock = 0; colBlock < numBlocks; ++colBlock) {
            // выражаем блоки через строки
            int rowStart = rowBlock * k;
            int colStart = colBlock * k;
            // учитывая крайние блоки
            int rowLen = std::min(k, n - rowStart);
            int colLen = std::min(k, n - colStart);
            // итоговый блок
            std::vector<std::vector<int>> block(rowLen, std::vector<int>(colLen, 0));
            // сумма произведений подблоков строк и столбцов, в которые входит рассматриваемый блок
            for (int innerBlock = 0; innerBlock < numBlocks; ++innerBlock) {
                int innerStart = innerBlock * k;
                // учитывая крайние блоки
                int midLen = std::min(k, n - innerStart);
                // соотв блоки строки и столбца из исходных матриц
                std::vector<std::vector<int>> aBlock(rowLen, std::vector<int>(midLen));
                std::vector<std::vector<int>> bBlock(midLen, std::vector<int>(colLen));
                for (int i = 0; i < rowLen; ++i)
                    for (int m = 0; m < midLen; ++m)
                        aBlock[i][m] = a[rowStart + i][innerStart + m];
                for (int m = 0; m < midLen; ++m)
                    for (int j = 0; j < colLen; ++j)
                        bBlock[m][j] = b[innerStart + m][colStart + j];
                // умножаем все соотв блоки и складываем 
                std::vector<std::vector<int>> blockResult = multiplyMatrices(aBlock, bBlock);
                for (int i = 0; i < rowLen; ++i)
                    for (int j = 0; j < colLen; ++j)
                        block[i][j] += blockResult[i][j];
            }
            // копируем блок результата в итоговую матрицу
            for (int i = 0; i < rowLen; ++i)
                for (int j = 0; j < colLen; ++j)
                    result[rowStart + i][colStart + j] = block[i][j];
        }
    }
    return result;
}


void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int value : row)
            std::cout << value << ' ';
        std::cout << '\n';
    }
}
