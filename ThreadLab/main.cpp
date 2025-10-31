#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>

using Matrix = std::vector<std::vector<double>>;

// Функция для умножения части строк
void multiplyPart(const Matrix& A, const Matrix& B, Matrix& C,
    size_t rowStart, size_t rowEnd) {
    size_t n = A[0].size();    // столбцы A = строки B
    size_t m = B[0].size();    // столбцы B

    for (size_t i = rowStart; i < rowEnd; ++i) {
        for (size_t j = 0; j < m; ++j) {
            double sum = 0;
            for (size_t k = 0; k < n; ++k)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
}

// Функция умножения матриц с использованием потоков
Matrix multiplyParallel(const Matrix& A, const Matrix& B, unsigned int threadsCount) {
    size_t n = A.size();
    size_t m = B[0].size();
    size_t k = B.size();

    if (A[0].size() != k)
        throw std::runtime_error("Matrix dimensions mismatch!");

    Matrix C(n, std::vector<double>(m, 0));

    // Разделяем строки по потокам
    size_t rowsPerThread = (n + threadsCount - 1) / threadsCount;
    std::vector<std::jthread> threads;

    for (unsigned int t = 0; t < threadsCount; ++t) {
        size_t rowStart = t * rowsPerThread;
        if (rowStart >= n) break;
        size_t rowEnd = std::min(rowStart + rowsPerThread, n);

        threads.emplace_back(multiplyPart, std::cref(A), std::cref(B), std::ref(C), rowStart, rowEnd);
    }

    // std::jthread автоматически join-ится в деструкторе
    return C;
}

int main() {
    size_t n, m, p;
    unsigned int threadsCount = std::thread::hardware_concurrency();

    std::cout << "Input Matrix sizes (A: n x m, B: m x p): ";
    std::cin >> n >> m >> p;

    Matrix A(n, std::vector<double>(m));
    Matrix B(m, std::vector<double>(p));

    std::cout << "Input Matrix A (" << n << "x" << m << "):\n";
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < m; ++j)
            std::cin >> A[i][j];

    std::cout << "Input Matrix B (" << m << "x" << p << "):\n";
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < p; ++j)
            std::cin >> B[i][j];

    auto start = std::chrono::high_resolution_clock::now();
    Matrix C = multiplyParallel(A, B, threadsCount);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "\nResult (A * B):\n";
    for (auto& row : C) {
        for (double val : row)
            std::cout << val << ' ';
        std::cout << '\n';
    }

    std::chrono::duration<double> duration = end - start;
    std::cout << "\nComputing time: " << duration.count() << " second.\n";
    std::cout << "Threads are used: " << threadsCount << "\n";

    return 0;
}
