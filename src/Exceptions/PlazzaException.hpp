/**
 * @file PlazzaException.hpp
 * @brief Defines custom exceptions for the Plazza.
 */

#pragma once

#include <exception>
#include <string>

namespace Plazza::Exceptions {

/**
 * @class PlazzaException
 * @brief Base class for all Plazza exceptions.
 */
class PlazzaException : public std::exception {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  explicit PlazzaException(std::string message) noexcept
      : m_message(std::move(message)) {}

  /**
   * @brief Get the error message.
   * @return C-string containing the error message.
   */
  [[nodiscard]] const char *what() const noexcept override {
    return m_message.c_str();
  }

protected:
  std::string m_message;
};
} // namespace Plazza::Exceptions
