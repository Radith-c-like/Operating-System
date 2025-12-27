#include <iostream>
#include <chrono>
#include <random>
#include "matrix_mult.h"

Matrix generateMatrix(size_t rows, size_t cols) {
    Matrix res(rows, std::vector<double>(cols));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 2.0);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            res[i][j] = dis(gen);
    return res;
}

int main() {
    size_t n, m, p;
    std::cout << "Enter matrix sizes (n m p): ";
    if (!(std::cin >> n >> m >> p)) return 1;

    Matrix A = generateMatrix(n, m);
    Matrix B = generateMatrix(m, p);

    unsigned int tRows = 4, tCols = 4;
    std::cout << "\n--- Starting Benchmarks ---\n";

    {
        auto start = std::chrono::high_resolution_clock::now();
        Matrix C = multiplyParallel2D(A, B, tRows, tCols);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "C++ Standard (jthread): " << std::chrono::duration<double>(end - start).count() << "s\n";
    }

#ifdef _WIN32
    {
        auto start = std::chrono::high_resolution_clock::now();
        Matrix C = multiplyParallelWinAPI(A, B, tRows, tCols);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Windows API:           " << std::chrono::duration<double>(end - start).count() << "s\n";
    }
#endif

#ifdef __linux__
    {
        auto start = std::chrono::high_resolution_clock::now();
        Matrix C = multiplyParallelPthread(A, B, tRows, tCols);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "POSIX Threads (Linux): " << std::chrono::duration<double>(end - start).count() << "s\n";
    }
#endif

    return 0;
}