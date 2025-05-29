/**
 * @file MessageException.hpp
 * @brief Defines custom exceptions for the Plazza messages.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class MessageException
 * @brief Base class for message-related exceptions.
 */
class MessageException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  MessageException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Message exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
