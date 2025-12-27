#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include <vector>

// Псевдоним для удобства
using Matrix = std::vector<std::vector<double>>;

/**
 * Параллельное умножение матриц с использованием 2D-декомпозиции (std::jthread)
 * @param threadsRows - на сколько частей разбить строки
 * @param threadsCols - на сколько частей разбить столбцы
 */
Matrix multiplyParallel2D(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);

#endif