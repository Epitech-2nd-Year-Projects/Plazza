/**
 * @file OpaqueObjectException.hpp
 * @brief Defines custom exceptions for the Plazza OpaqueObject class.
 */

#pragma once

#include "Exceptions/PlazzaException.hpp"
#include <string>

namespace Plazza::Exceptions {

/**
 * @class OpaqueObjectException
 * @brief Base class for opaque object-related exceptions.
 */
class OpaqueObjectException final : public PlazzaException {
public:
  /**
   * @brief Constructor with error message.
   * @param message Error description.
   */
  OpaqueObjectException(std::string message) noexcept
      : PlazzaException(std::move(message)) {
    m_message = "Opaque Object exception: '" + m_message;
  }
};
} // namespace Plazza::Exceptions
