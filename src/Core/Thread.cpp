/**
 * @file Thread.cpp
 * @brief Implements the Thread class for managing threads.
 */

#include "Core/Thread.hpp"
#include "Exceptions/ThreadException.hpp"

namespace Plazza::Core {
Thread::~Thread() {
  if (m_thread.joinable()) {
    stop();
    join();
  }
}

Thread::Thread(Thread &&other) noexcept
    : m_thread(std::move(other.m_thread)),
      m_shouldStop(other.m_shouldStop.load()) {}

Thread &Thread::operator=(Thread &&other) noexcept {
  if (this != &other) {
    if (m_thread.joinable()) {
      stop();
      join();
    }
    m_thread = std::move(other.m_thread);
    m_shouldStop = other.m_shouldStop.load();
  }
  return *this;
}

void Thread::start(std::function<void()> threadTask) {
  if (m_thread.joinable()) {
    throw Exceptions::ThreadException(
        "Thread is already running, cannot start again.");
  }

  m_shouldStop = false;
  m_thread = std::thread([threadTask = std::move(threadTask)]() {
    try {
      threadTask();
    } catch (const std::exception &e) {
    }
  });
}

void Thread::join() {
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void Thread::detach() {
  if (m_thread.joinable()) {
    m_thread.detach();
  }
}

bool Thread::joinable() const { return m_thread.joinable(); }

void Thread::stop() { m_shouldStop = true; }
} // namespace Plazza::Core
