/**
 * @file ThreadQueue.hpp
 * @brief Defines the ThreadQueue class for thread-safe queue management.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

namespace Plazza::Core {
/**
 * @class ThreadQueue
 * @brief A thread-safe queue that allows multiple threads to push and pop
 * items safely.
 * @tparam T The type of items stored in the queue.
 */
template <typename T> class ThreadQueue {
public:
  /**
   * @brief Pushes an item into the queue.
   * @param item The item to be pushed into the queue.
   */
  void push(T item) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(std::move(item));
    m_condition.notify_one();
  }

  /**
   * @brief Attempts to pop an item from the queue.
   * @return An optional containing the popped item if successful, or
   * std::nullopt if the queue is empty.
   */
  std::optional<T> tryPop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty()) {
      return std::nullopt;
    }
    T result = std::move(m_queue.front());
    m_queue.pop();
    return result;
  }

  /**
   * @brief Checks if the queue is empty.
   * @return True if the queue is empty, false otherwise.
   */
  bool empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
  }

  /**
   * @brief Gets the current size of the queue.
   * @return The number of items in the queue.
   */
  std::size_t size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
  }

private:
  mutable std::mutex m_mutex;
  std::queue<T> m_queue;
  std::condition_variable m_condition;
};

} // namespace Plazza::Core
