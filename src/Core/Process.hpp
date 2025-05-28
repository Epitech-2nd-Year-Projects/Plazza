/**
 * @file Process.hpp
 * @brief Defines the Process class for managing processes.
 */

#pragma once

#include <functional>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Plazza::Core {
/**
 * @class Process
 * @brief Encapsulates process management.
 */
class Process {
public:
  /**
   * @brief Default constructor.
   */
  Process() = default;

  /**
   * @brief Destructor that ensures the process is terminated if it was forked.
   */
  ~Process();

  Process(const Process &) = delete;
  Process &operator=(const Process &) = delete;

  /**
   * @brief Move constructor.
   * @param other The Process object to move from.
   */
  Process(Process &&other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The Process object to move from.
   * @return Reference to this Process object.
   */
  Process &operator=(Process &&other) noexcept;

  /**
   * @brief Forks a new process and runs the provided function in the child
   * process.
   * @param processFunction The function to run in the child process.
   * @throws Exceptions::ProcessException if the fork fails.
   */
  void fork(std::function<void()> processFunction);

  /**
   * @brief Waits for the forked process to finish.
   */
  void wait();

  /**
   * @brief Checks if the process is currently running.
   * @return True if the process is running, false otherwise.
   */
  [[nodiscard]] bool isRunning() const;

  /**
   * @brief Gets the process ID of the forked process.
   * @return The process ID.
   */
  [[nodiscard]] pid_t getPid() const { return m_pid; }

  /**
   * @brief Terminates the forked process.
   */
  void terminate();

private:
  pid_t m_pid = -1;
  bool m_forked = false;
};
} // namespace Plazza::Core
