/**
 * @file MessageQueue.hpp
 * @brief Defines the MessageQueue class for inter-process communication using
 * POSIX message queues.
 */

#pragma once

#include <chrono>
#include <mqueue.h>
#include <optional>
#include <string>

namespace Plazza::Communication {
/**
 * @class MessageQueue
 * @brief A class for managing a POSIX message queue for inter-process
 * communication.
 */
class MessageQueue {
public:
  /**
   * @brief Constructs a MessageQueue instance.
   * @param queueName The name of the message queue.
   * @param isCreator If true, the queue will be created, otherwise, it will be
   * opened if it already exists.
   */
  MessageQueue(const std::string &queueName, bool isCreator = false);

  /**
   * @brief Destructor that closes the message queue.
   */
  ~MessageQueue();

  MessageQueue(const MessageQueue &) = delete;
  MessageQueue &operator=(const MessageQueue &) = delete;

  /**
   * @brief Move constructor.
   * @param other The MessageQueue object to move from.
   */
  MessageQueue(MessageQueue &&other) noexcept;

  /**
   * @brief Move assignment operator.
   * @param other The MessageQueue object to move from.
   * @return Reference to this MessageQueue object.
   */
  MessageQueue &operator=(MessageQueue &&other) noexcept;

  /**
   * @brief Sends a message to the message queue.
   * @param message The message to send.
   * @param priority The priority of the message.
   * @throws Exceptions::MessageException if sending fails.
   */
  void send(const std::string &message, unsigned int priority = 0);

  /**
   * @brief Receives a message from the message queue.
   * @return The received message, or std::nullopt if no message is available.
   * @throws Exceptions::MessageException if receiving fails.
   */
  std::optional<std::string> receive();

  /**
   * @brief Receives a message from the message queue with a timeout.
   * @param timeout The maximum time to wait for a message.
   * @return The received message, or std::nullopt if no message is available
   * within the timeout.
   * @throws Exceptions::MessageException if receiving fails.
   */
  std::optional<std::string> timedReceive(std::chrono::milliseconds timeout);

  /**
   * @brief Checks if the message queue is valid.
   * @return True if the message queue is valid, false otherwise.
   */
  [[nodiscard]] bool isValid() const { return m_descriptor != -1; }

  /**
   * @brief Closes the message queue.
   * @throws Exceptions::MessageException if closing fails.
   */
  void close();

  /**
   * @brief Overloaded operator for sending messages.
   * @tparam T The type of the data to send.
   * @param data The data to send.
   * @return Reference to this MessageQueue object.
   */
  template <typename T> MessageQueue &operator<<(const T &data);

  /**
   * @brief Overloaded operator for receiving messages.
   * @tparam T The type of the data to receive.
   * @param data The data to receive.
   * @return Reference to this MessageQueue object.
   */
  template <typename T> MessageQueue &operator>>(T &data);

private:
  static constexpr size_t MAX_MESSAGE_SIZE = 8192;
  static constexpr int MAX_MESSAGES = 100;

  std::string m_name;
  mqd_t m_descriptor;
  bool m_isCreator;
  bool m_isOpen;
};

template <typename T> MessageQueue &MessageQueue::operator<<(const T &data) {
  std::ostringstream oss;
  oss << data;
  send(oss.str());
  return *this;
}

template <typename T> MessageQueue &MessageQueue::operator>>(T &data) {
  std::optional<std::string> message = receive();
  if (message) {
    std::istringstream iss(*message);
    iss >> data;
  }
  return *this;
}
} // namespace Plazza::Communication
