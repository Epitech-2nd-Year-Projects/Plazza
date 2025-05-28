/**
 * @file ThreadException.hpp
 * @brief Defines custom exceptions for the Plazza threads.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class ThreadException
 * @brief Base class for thread-related exceptions.
 */
class ThreadException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  ThreadException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Thread exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
