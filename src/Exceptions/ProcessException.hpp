/**
 * @file ProcessException.hpp
 * @brief Defines custom exceptions for the Plazza processes.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class ProcessException
 * @brief Base class for process-related exceptions.
 */
class ProcessException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  ProcessException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Process exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
