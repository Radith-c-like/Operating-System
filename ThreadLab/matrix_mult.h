#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Кроссплатформенная версия (C++20) - доступна везде
Matrix multiplyParallel2D(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);

#ifdef _WIN32
// Доступно только на Windows
Matrix multiplyParallelWinAPI(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);
#endif

#ifdef __linux__
// Доступно только на Linux
Matrix multiplyParallelPthread(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);
#endif

#endif