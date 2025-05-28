/**
 * @file Thread.hpp
 * @brief Defines the Thread class for managing threads.
 */

#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace Plazza::Core {
/**
 * @class Thread
 * @brief Encapsulates thread management.
 */
class Thread {
public:
  /**
   * @brief Default constructor.
   */
  Thread() = default;

  /**
   * @brief Destructor that ensures the thread is joined or detached if it was
   * started.
   */
  ~Thread();

  Thread(const Thread &) = delete;
  Thread &operator=(const Thread &) = delete;

  /**
   * @brief Move constructor.
   * @param other The Thread object to move from.
   */
  Thread(Thread &&other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The Thread object to move from.
   * @return Reference to this Thread object.
   */
  Thread &operator=(Thread &&other) noexcept;

  /**
   * @brief Starts the thread with the provided task.
   * @param threadTask The function to run in the thread.
   * @throws Exceptions::ThreadException if the thread is already running.
   */
  void start(std::function<void()> threadTask);

  /**
   * @brief Joins the thread, blocking until it finishes.
   */
  void join();

  /**
   * @brief Detaches the thread, allowing it to run independently.
   */
  void detach();

  /**
   * @brief Gets the thread ID.
   * @return The ID of the thread.
   */
  bool joinable() const;

  /**
   * @brief Stops the thread by setting the shouldStop flag to true.
   */
  void stop();

private:
  std::thread m_thread;
  std::atomic<bool> m_shouldStop{false};
};
} // namespace Plazza::Core
