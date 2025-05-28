/**
 * @file Process.cpp
 * @brief Implements the Process class for managing processes.
 */

#include "Core/Process.hpp"
#include "Exceptions/ProcessException.hpp"
#include <signal.h>
#include <sys/wait.h>

namespace Plazza::Core {

Process::~Process() {
  if (m_forked && m_pid > 0) {
    terminate();
  }
}

Process::Process(Process &&other) noexcept
    : m_pid(other.m_pid), m_forked(other.m_forked) {
  other.m_pid = -1;
  other.m_forked = false;
}

Process &Process::operator=(Process &&other) noexcept {
  if (this != &other) {
    if (m_forked && m_pid > 0) {
      terminate();
    }
    m_pid = other.m_pid;
    m_forked = other.m_forked;
    other.m_pid = -1;
    other.m_forked = false;
  }
  return *this;
}

void Process::fork(std::function<void()> processFunction) {
  m_pid = ::fork();

  if (m_pid == -1) {
    throw Exceptions::ProcessException("Failed to fork process");
  }

  if (m_pid == 0) {
    try {
      processFunction();
      std::exit(0);
    } catch (...) {
      std::exit(1);
    }
  } else {
    m_forked = true;
  }
}

void Process::wait() {
  if (m_forked && m_pid > 0) {
    int status;
    ::waitpid(m_pid, &status, 0);
    m_forked = false;
    m_pid = -1;
  }
}

bool Process::isRunning() const {
  if (!m_forked || m_pid <= 0) {
    return false;
  }

  int status;

  pid_t result = ::waitpid(m_pid, &status, WNOHANG);
  return result == 0;
}

void Process::terminate() {
  if (m_forked && m_pid > 0) {
    ::kill(m_pid, SIGTERM);
    wait();
  }
}
} // namespace Plazza::Core
