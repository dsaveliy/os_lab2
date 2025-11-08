#pragma once
#include <vector>
#include <fstream>
#include <iostream>

// создать рандомную
std::vector<std::vector<int>> generateMatrix(int N);

// чтение
std::vector<std::vector<int>> readMatrixFromFile(const std::string& filename);

// вывод
void printMatrix(const std::vector<std::vector<int>>& matrix);

// умножение матриц (стандартное)
std::vector<std::vector<int>> multiplyMatrices(const std::vector<std::vector<int>>& aBlock,
    const std::vector<std::vector<int>>& bBlock);

// умножение матриц блочным методом
std::vector<std::vector<int>> multiplyMatricesByBlocks( const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b, int k);