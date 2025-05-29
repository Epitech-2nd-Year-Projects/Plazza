/**
 * @file Message.hpp
 * @brief Defines the Message class for representing different types of
 * messages.
 */

#pragma once

#include <cstdint>
#include <string>

namespace Plazza::Communication {
/**
 * @class Message
 * @brief A class for representing different types of messages used in the
 * Plazza communication system.
 */
class Message {
public:
  enum class MessageType : uint8_t {
    PIZZA_ORDER = 1,
    PIZZA_COMPLETED = 2,
    STATUS_REQUEST = 3,
    STATUS_RESPONSE = 4,
    SHUTDOWN = 5,
    HEARTBEAT = 6
  };

  /**
   * @brief Constructs a Message object.
   * @param type The type of the message.
   * @param senderId The ID of the sender.
   * @param timestamp The timestamp of the message.
   * @param payload The payload of the message.
   */
  Message(MessageType type, uint32_t senderId, uint32_t timestamp,
          std::string payload);

  /**
   * @brief Serializes the message to a string format.
   */
  std::string serialize() const;

  /**
   * @brief Deserializes a string to create a Message object.
   * @param data The serialized message data.
   * @return A Message object created from the serialized data.
   * @throws Exceptions::MessageException if the data is invalid.
   */
  static Message deserialize(const std::string &data);

private:
  MessageType m_type;
  uint32_t m_senderId;
  uint32_t m_timestamp;
  std::string m_payload;
};
} // namespace Plazza::Communication
