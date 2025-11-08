#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "matrix.h"
#include "pthread.h"
#include "stdthread.h"

void runTest(int N, std::vector<int> kValues) {
    std::cout << "\nMatrix size: " << N << "x" << N << "\n";
    auto A = generateMatrix(N);
    auto B = generateMatrix(N);

    auto start = std::chrono::high_resolution_clock::now();
    auto classicResult = multiplyMatrices(A, B);
    auto end = std::chrono::high_resolution_clock::now();
    double ms_classic = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "\nClassic: Time(ms) = " << std::fixed << std::setprecision(1) << ms_classic << std::endl;

    std::cout << "\nk\tthreads\t\tstd::thread\tpthread\n";

    for (int k : kValues) {
        int numBlocks = N / k + (N % k ? 1 : 0);
        int numThreads = numBlocks * numBlocks;

        start = std::chrono::high_resolution_clock::now();
        auto tResult = multiplyMatricesByBlocksThread(A, B, k);
        end = std::chrono::high_resolution_clock::now();
        double ms_thread = std::chrono::duration<double, std::milli>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        auto pResult = multiplyMatricesByBlocksPthread(A, B, k);
        end = std::chrono::high_resolution_clock::now();
        double ms_pthread = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << k << '\t' << numThreads << "\t\t" << std::fixed << std::setprecision(1)
                  << ms_thread << "\t\t" << ms_pthread << std::endl;
    }
}

int main() {
    runTest(20,  { 20, 10, 7, 5, 4, 3 });
    runTest(100, { 100, 50, 34, 25, 20, 17, 13, 12 });
    runTest(200, { 200, 100, 67, 50, 40, 34, 29, 25, 23 });
    return 0;
}
