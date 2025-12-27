#include "matrix_mult.h"
#include <thread>
#include <algorithm>
#include <functional>
#include <stdexcept>

void multiplyBlock(const Matrix& A, const Matrix& B, Matrix& C,
    size_t rowStart, size_t rowEnd,
    size_t colStart, size_t colEnd) {
    size_t n_inner = A[0].size();

    for (size_t i = rowStart; i < rowEnd; ++i) {
        for (size_t k = 0; k < n_inner; ++k) {
            double temp = A[i][k];
            for (size_t j = colStart; j < colEnd; ++j) {
                C[i][j] += temp * B[k][j];
            }
        }
    }
}

Matrix multiplyParallel2D(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols) {
    size_t n = A.size();
    size_t m = A[0].size();
    size_t p = B[0].size();

    if (m != B.size())
        throw std::runtime_error("Matrix dimensions mismatch!");

    Matrix C(n, std::vector<double>(p, 0.0));

    size_t blockRows = (n + threadsRows - 1) / threadsRows;
    size_t blockCols = (p + threadsCols - 1) / threadsCols;

    std::vector<std::jthread> threads;

    for (unsigned int r = 0; r < threadsRows; ++r) {
        for (unsigned int c = 0; c < threadsCols; ++c) {
            size_t rowStart = r * blockRows;
            size_t rowEnd = std::min(rowStart + blockRows, n);
            size_t colStart = c * blockCols;
            size_t colEnd = std::min(colStart + blockCols, p);

            if (rowStart >= n || colStart >= p) continue;

            threads.emplace_back(multiplyBlock,
                std::cref(A), std::cref(B), std::ref(C),
                rowStart, rowEnd, colStart, colEnd);
        }
    }

    return C;
}