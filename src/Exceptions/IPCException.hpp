/**
 * @file IPCException.hpp
 * @brief Defines custom exceptions for the Plazza IPC.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class IPCException
 * @brief Base class for ipc-related exceptions.
 */
class IPCException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  IPCException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "IPC exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
