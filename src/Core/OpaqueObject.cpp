#include "Core/OpaqueObject.hpp"
#include "Exceptions/OpaqueObjectException.hpp"
#include <iomanip>
#include <sstream>

namespace Plazza::Core {
OpaqueObject::OpaqueObject(std::vector<uint8_t> data)
    : m_data(std::move(data)), m_readOffset(0) {}

void OpaqueObject::clear() {
  m_data.clear();
  m_readOffset = 0;
}

void OpaqueObject::reset() { m_readOffset = 0; }

std::string OpaqueObject::toString() const {
  std::ostringstream oss;
  for (uint8_t byte : m_data) {
    oss << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(byte);
  }
  return oss.str();
}

OpaqueObject OpaqueObject::fromString(const std::string &payload) {
  if (payload.length() % 2 != 0) {
    throw Exceptions::OpaqueObjectException(
        "Invalid hex string: length must be even");
  }

  std::vector<uint8_t> data;
  data.reserve(payload.length() / 2);

  for (size_t i = 0; i < payload.length(); i += 2) {
    std::string byteString = payload.substr(i, 2);
    uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
    data.push_back(byte);
  }

  return OpaqueObject(std::move(data));
}
} // namespace Plazza::Core
