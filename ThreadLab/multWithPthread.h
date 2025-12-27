#pragma once
#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include <vector>

using Matrix = std::vector<std::vector<double>>;

// Функция для вызова из main
Matrix multiplyParallelPthread(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);

#endif