/**
 * @file ArgumentException.hpp
 * @brief Defines custom exceptions for the Plazza arguments.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class ArgumentException
 * @brief Base class for argument-related exceptions.
 */
class ArgumentException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  ArgumentException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Argument exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
