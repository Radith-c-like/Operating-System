#ifdef __linux__
#include <pthread.h>

#include "matrix_mult.h"
#include <algorithm>
#include <stdexcept>

struct ThreadArgs {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    size_t rowStart, rowEnd, colStart, colEnd;
};

void* multiplyBlockPthread(void* arg) {
    ThreadArgs* data = static_cast<ThreadArgs*>(arg);
    size_t n_inner = (*data->A)[0].size();

    for (size_t i = data->rowStart; i < data->rowEnd; ++i) {
        for (size_t k = 0; k < n_inner; ++k) {
            double temp = (*data->A)[i][k];
            for (size_t j = data->colStart; j < data->colEnd; ++j) {
                (*data->C)[i][j] += temp * (*data->B)[k][j];
            }
        }
    }
    return nullptr;
}

Matrix multiplyParallelPthread(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols) {
    size_t n = A.size();
    size_t p = B[0].size();

    if (A[0].size() != B.size())
        throw std::runtime_error("Dimensions mismatch!");

    Matrix C(n, std::vector<double>(p, 0.0));

    size_t blockRows = (n + threadsRows - 1) / threadsRows;
    size_t blockCols = (p + threadsCols - 1) / threadsCols;

    std::vector<pthread_t> threads;
    std::vector<ThreadArgs*> argsList;

    for (unsigned int r = 0; r < threadsRows; ++r) {
        for (unsigned int c = 0; c < threadsCols; ++c) {
            size_t rowStart = r * blockRows;
            size_t rowEnd = std::min(rowStart + blockRows, n);
            size_t colStart = c * blockCols;
            size_t colEnd = std::min(colStart + blockCols, p);

            if (rowStart >= n || colStart >= p) continue;

            ThreadArgs* args = new ThreadArgs{ &A, &B, &C, rowStart, rowEnd, colStart, colEnd };
            argsList.push_back(args);

            pthread_t thread;
            if (pthread_create(&thread, nullptr, multiplyBlockPthread, args) != 0) {
                throw std::runtime_error("Failed to create pthread");
            }
            threads.push_back(thread);
        }
    }

    for (pthread_t t : threads) {
        pthread_join(t, nullptr);
    }

    for (ThreadArgs* a : argsList) delete a;

    return C;
}

#endif
