#include "Communication/Message.hpp"
#include "Exceptions/MessageException.hpp"
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

namespace Plazza::Communication {
Message::Message(MessageType type, uint32_t senderId, uint32_t timestamp,
                 std::string payload)
    : m_type(type), m_senderId(senderId), m_timestamp(timestamp),
      m_payload(std::move(payload)) {}

std::string Message::serialize() const {
  std::ostringstream oss;
  oss << static_cast<uint8_t>(m_type) << "|" << m_senderId << "|" << m_timestamp
      << "|" << m_payload.length() << "|" << m_payload;
  return oss.str();
}

Message Message::deserialize(const std::string &data) {
  std::istringstream iss(data);
  std::string token;

  if (!std::getline(iss, token, '|')) {
    throw Exceptions::MessageException("Invalid message format");
  }
  MessageType type = static_cast<MessageType>(std::stoi(token));

  if (!std::getline(iss, token, '|')) {
    throw Exceptions::MessageException("Invalid message format");
  }
  uint32_t senderId = std::stoul(token);

  if (!std::getline(iss, token, '|')) {
    throw Exceptions::MessageException("Invalid message format");
  }
  uint32_t timestamp = std::stoul(token);

  if (!std::getline(iss, token, '|')) {
    throw Exceptions::MessageException("Invalid message format");
  }
  std::size_t payloadLength = std::stoul(token);

  std::string payload;
  if (payloadLength > 0) {
    payload.resize(payloadLength);
    iss.read(&payload[0], payloadLength);
  }
  return Message(type, senderId, timestamp, std::move(payload));
}
} // namespace Plazza::Communication
