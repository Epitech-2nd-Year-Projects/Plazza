/**
 * @file ParserException.hpp
 * @brief Defines custom exceptions for the Plazza parser.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class ParserException
 * @brief Base class for parser-related exceptions.
 */
class ParserException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  ParserException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Parser exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
