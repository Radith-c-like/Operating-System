#pragma once
#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include <vector>
#include <windows.h>

using Matrix = std::vector<std::vector<double>>;

Matrix multiplyParallelWinAPI(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols);

#endif