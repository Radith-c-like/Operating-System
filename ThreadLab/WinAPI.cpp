#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <clocale>
#include <algorithm>

class MatrixSync {
private:
    size_t n_;
    std::vector<double> data_;
    mutable CRITICAL_SECTION cs_data;

public:
    explicit MatrixSync(size_t n) : n_(n), data_(n* n, 0.0) {
        InitializeCriticalSection(&cs_data);
    }

    ~MatrixSync() {
        DeleteCriticalSection(&cs_data);
    }

    MatrixSync(const MatrixSync&) = delete;
    MatrixSync& operator=(const MatrixSync&) = delete;

    
    MatrixSync(MatrixSync&& other) noexcept
        : n_(other.n_), data_(std::move(other.data_)) {
        InitializeCriticalSection(&cs_data);
    }

    size_t size() const { return n_; }

    double retrieve(size_t i, size_t j) const {
        if (i >= n_ || j >= n_) return 0.0;
        return data_[i * n_ + j];
    }

    void assign(size_t i, size_t j, double val) {
        if (i < n_ && j < n_) {
            data_[i * n_ + j] = val;
        }
    }

    void accumulate_protected(size_t i, size_t j, double val) {
        if (i >= n_ || j >= n_) return;

        EnterCriticalSection(&cs_data);
        data_[i * n_ + j] += val;
        LeaveCriticalSection(&cs_data);
    }

    void populate_random() {
        std::mt19937 gen(17);
        std::uniform_real_distribution<double> dist(0.0, 10.0);

        for (size_t r = 0; r < n_; ++r) {
            for (size_t c = 0; c < n_; ++c) {
                assign(r, c, dist(gen));
            }
        }
    }
};

struct BlockTask {
    const MatrixSync* src_a;
    const MatrixSync* src_b;
    MatrixSync* dst_res;
    size_t start_row;
    size_t start_col_a;
    size_t start_col_b;
    size_t block_dim;
    size_t mat_dim;
};

static DWORD WINAPI process_block_worker(LPVOID lpParam) {
    BlockTask* task = static_cast<BlockTask*>(lpParam);

    size_t r_limit = (std::min)(task->start_row + task->block_dim, task->mat_dim);
    size_t c_limit = (std::min)(task->start_col_b + task->block_dim, task->mat_dim);
    size_t k_limit = (std::min)(task->start_col_a + task->block_dim, task->mat_dim);

    for (size_t r = task->start_row; r < r_limit; ++r) {
        for (size_t c = task->start_col_b; c < c_limit; ++c) {
            double local_sum = 0.0;
            for (size_t k = task->start_col_a; k < k_limit; ++k) {
                local_sum += task->src_a->retrieve(r, k) * task->src_b->retrieve(k, c);
            }
            task->dst_res->accumulate_protected(r, c, local_sum);
        }
    }

    delete task;
    return 0;
}

static long long execute_parallel_multiply(const MatrixSync& a, const MatrixSync& b,
    MatrixSync& result, size_t blk_size) {
    size_t n = a.size();
    std::vector<HANDLE> thread_handles;

    auto t_start = std::chrono::high_resolution_clock::now();

    for (size_t row = 0; row < n; row += blk_size) {
        for (size_t col = 0; col < n; col += blk_size) {
            for (size_t depth = 0; depth < n; depth += blk_size) {
                BlockTask* task = new BlockTask{
                    &a, &b, &result,
                    row, depth, col,
                    blk_size, n
                };

                HANDLE hThread = CreateThread(NULL, 0, process_block_worker, task, 0, NULL);
                if (hThread) {
                    thread_handles.push_back(hThread);
                }
            }
        }
    }

    for (size_t i = 0; i < thread_handles.size(); i += MAXIMUM_WAIT_OBJECTS) {
        DWORD batch_size = (std::min)(
            static_cast<DWORD>(thread_handles.size() - i),
            static_cast<DWORD>(MAXIMUM_WAIT_OBJECTS)
            );
        WaitForMultipleObjects(batch_size, &thread_handles[i], TRUE, INFINITE);
    }

    for (HANDLE h : thread_handles) CloseHandle(h);

    auto t_end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
}

static void multiply_linear(const MatrixSync& a, const MatrixSync& b, MatrixSync& c) {
    size_t n = a.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double s = 0.0;
            for (size_t k = 0; k < n; ++k) {
                s += a.retrieve(i, k) * b.retrieve(k, j);
            }
            c.assign(i, j, s);
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    size_t dim;
    std::cout << " -> Input Matrixes Sizes N (N x N): ";
    std::cin >> dim;

    if (dim < 1) {
        std::cout << " [!] Wrong Input. Set default : 100.\n";
        dim = 100;
    }

    std::cout << "\n[*] Init Matrix. Dimension is " << dim << "x" << dim << "...\n";

    MatrixSync mat_a(dim);
    MatrixSync mat_b(dim);
    mat_a.populate_random();
    mat_b.populate_random();

    std::cout << "\n[1] One Thread\n";

    MatrixSync res_linear(dim);
    auto start_linear = std::chrono::high_resolution_clock::now();

    multiply_linear(mat_a, mat_b, res_linear);

    long long time_linear = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - start_linear
    ).count();

    std::cout << "    Time spend: " << time_linear << " mks\n";

    std::cout << "\n[2] Multhithreading\n";
    std::cout << " " << std::string(60, '-') << "\n";
    std::cout << " | Block size | Threads Count| Time (mks) |\n";
    std::cout << " " << std::string(60, '-') << "\n";

    for (size_t blk_sz = 1; blk_sz <= dim; ) {
        MatrixSync res_block(dim);

        size_t blocks_per_side = (dim + blk_sz - 1) / blk_sz;
        size_t total_threads_est = blocks_per_side * blocks_per_side * blocks_per_side;

        long long time_block = execute_parallel_multiply(mat_a, mat_b, res_block, blk_sz);

        std::cout << " | " << std::setw(12) << blk_sz
            << " | " << std::setw(16) << total_threads_est
            << " | " << std::setw(11) << time_block << " |\n";

        if (blk_sz * 2 > dim) {
            if (blk_sz < dim) blk_sz = dim;
            else break;
        }
        else {
            blk_sz *= 2;
        }
    }

    std::cout << " " << std::string(60, '-') << "\n";
    std::cout << "\n[*] end.\n";

    return 0;
}