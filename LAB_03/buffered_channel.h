#ifndef BUFFERED_CHANNEL
#define BUFFERED_CHANNEL

#include <queue>
#include <stdexcept>
#include <mutex>
#include <condition_variable>

template<typename T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t buffer_size)
        : m_buffer_size(buffer_size), m_isClosed(false) {
    }

    void send(const T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_not_full.wait(lock, [this]() {
            return m_isClosed || m_buffer.size() < m_buffer_size;
            });

        if (m_isClosed) {
            throw std::runtime_error("Channel is closed");
        }

        m_buffer.push(value);
        m_not_empty.notify_one();
    }

    std::pair<T, bool> recv() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_not_empty.wait(lock, [this]() {
            return m_isClosed || !m_buffer.empty();
            });

        if (!m_buffer.empty()) {
            T value = std::move(m_buffer.front());
            m_buffer.pop();
            m_not_full.notify_one();
            return { std::move(value), true };
        }

        return { T(), false };
    }

    void close() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_isClosed = true;
        m_not_empty.notify_all();
        m_not_full.notify_all();
    }

private:
    bool m_isClosed;
    std::mutex m_mutex;
    std::condition_variable m_not_full;
    std::condition_variable m_not_empty;
    std::queue<T> m_buffer;
    size_t m_buffer_size;
};

#endif