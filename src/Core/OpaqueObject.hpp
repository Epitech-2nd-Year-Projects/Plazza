/**
 * @file OpaqueObject.hpp
 * @brief Defines the OpaqueObject class for handling binary data.
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace Plazza::Core {
/**
 * @class OpaqueObject
 * @brief A class for handling binary data in a flexible way.
 */
class OpaqueObject {
public:
  /**
   * @brief Default constructor.
   */
  OpaqueObject() = default;

  /**
   * @brief Constructs an OpaqueObject with initial data.
   * @param data The initial binary data.
   */
  explicit OpaqueObject(std::vector<uint8_t> data);

  /**
   * @brief Packs a value into the OpaqueObject.
   * @tparam T The type of the value to pack.
   * @param value The value to pack.
   * @return Reference to this OpaqueObject.
   */
  template <typename T> OpaqueObject &pack(T value) {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Type must be trivially copyable");
    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&value);
    m_data.insert(m_data.end(), ptr, ptr + sizeof(T));
    return *this;
  }

  /**
   * @brief Unpacks a value from the OpaqueObject.
   * @tparam T The type of the value to unpack.
   * @param value The variable to store the unpacked value.
   * @return Reference to this OpaqueObject.
   */
  template <typename T> OpaqueObject &unpack(T &value) {
    static_assert(std::is_trivially_copyable_v<T>,
                  "Type must be trivially copyable");
    if (m_readOffset + sizeof(T) > m_data.size()) {
      throw std::runtime_error("Not enough data to unpack");
    }
    std::memcpy(&value, &m_data[m_readOffset], sizeof(T));
    m_readOffset += sizeof(T);
    return *this;
  }

  /**
   * @brief Gets the internal data vector.
   * @return A constant reference to the internal data vector.
   */
  const std::vector<uint8_t> &getData() const { return m_data; }

  /**
   * @brief Gets the internal data vector (non-const version).
   * @return A reference to the internal data vector.
   */
  std::vector<uint8_t> &getData() { return m_data; }

  /**
   * @brief Gets the current internal data size.
   * @return The current internal data size.
   */
  size_t size() const { return m_data.size(); }

  /**
   * @brief Checks if the OpaqueObject is empty.
   * @return True if the OpaqueObject is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const { return m_data.empty(); }

  /**
   * @brief Clears the internal data.
   */
  void clear();

  /**
   * @brief Resets the read offset to zero.
   */
  void reset();

  /**
   * @brief Converts the internal data to a hexadecimal string.
   * @return A string representation of the internal data in hexadecimal format.
   */
  std::string toString() const;

  /**
   * @brief Creates an OpaqueObject from a hexadecimal string.
   * @param str The hexadecimal string to convert.
   * @return An OpaqueObject created from the string.
   * @throws std::runtime_error if the string is not valid.
   */
  static OpaqueObject fromString(const std::string &payload);

private:
  std::vector<uint8_t> m_data;
  mutable size_t m_readOffset = 0;
};

/**
 * @brief Overloaded operator for packing values into an OpaqueObject.
 * @tparam T The type of the value to pack.
 * @param object The OpaqueObject to pack into.
 * @param value The value to pack.
 * @return Reference to the OpaqueObject.
 */
template <typename T>
OpaqueObject &operator<<(OpaqueObject &object, const T &value) {
  return object.pack(value);
}

/**
 * @brief Overloaded operator for unpacking values from an OpaqueObject.
 * @tparam T The type of the value to unpack.
 * @param object The OpaqueObject to unpack from.
 * @param value The variable to store the unpacked value.
 * @return Reference to the OpaqueObject.
 */
template <typename T> OpaqueObject &operator>>(OpaqueObject &object, T &value) {
  return object.unpack(value);
}
} // namespace Plazza::Core
