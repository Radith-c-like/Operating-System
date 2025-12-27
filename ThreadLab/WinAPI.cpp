#ifdef _WIN32
#include <windows.h>

#include "matrix_mult.h"
#include <algorithm>


struct ThreadData {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    size_t rowStart, rowEnd, colStart, colEnd;
};

DWORD WINAPI multiplyBlockWin(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    size_t n_inner = (*data->A)[0].size();

    for (size_t i = data->rowStart; i < data->rowEnd; ++i) {
        for (size_t k = 0; k < n_inner; ++k) {
            double temp = (*data->A)[i][k];
            for (size_t j = data->colStart; j < data->colEnd; ++j) {
                (*data->C)[i][j] += temp * (*data->B)[k][j];
            }
        }
    }
    return 0;
}

Matrix multiplyParallelWinAPI(const Matrix& A, const Matrix& B,
    unsigned int threadsRows, unsigned int threadsCols) {
    size_t n = A.size();
    size_t p = B[0].size();

    Matrix C(n, std::vector<double>(p, 0.0));

    size_t blockRows = (n + threadsRows - 1) / threadsRows;
    size_t blockCols = (p + threadsCols - 1) / threadsCols;

    std::vector<HANDLE> hThreads;
    std::vector<ThreadData*> threadArgs;

    for (unsigned int r = 0; r < threadsRows; ++r) {
        for (unsigned int c = 0; c < threadsCols; ++c) {
            size_t rowStart = r * blockRows;
            size_t rowEnd = (std::min)(rowStart + blockRows, n);
            size_t colStart = c * blockCols;
            size_t colEnd = (std::min)(colStart + blockCols, p);

            if (rowStart >= n || colStart >= p) continue;

            ThreadData* data = new ThreadData{ &A, &B, &C, rowStart, rowEnd, colStart, colEnd };
            threadArgs.push_back(data);

            HANDLE hThread = CreateThread(NULL, 0, multiplyBlockWin, data, 0, NULL);
            if (hThread != NULL) hThreads.push_back(hThread);
        }
    }

    if (!hThreads.empty()) {
        WaitForMultipleObjects((DWORD)hThreads.size(), hThreads.data(), TRUE, INFINITE);
    }

    for (HANDLE h : hThreads) CloseHandle(h);
    for (ThreadData* data : threadArgs) delete data;

    return C;
}

#endif

