/**
 * @file IPCManager.hpp
 * @brief Defines the IPCManager class for managing IPC.
 */

#pragma once

#include "Communication/Message.hpp"
#include "Communication/MessageQueue.hpp"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace Plazza::Communication {
/**
 * @class IPCManager
 * @brief A class for managing IPC.
 */
class IPCManager {
public:
  /**
   * @brief Constructs an IPCManager instance.
   * @param id The ID of the IPCManager.
   * @param isReception If true, this instance acts as a reception manager.
   */
  IPCManager(uint32_t id, bool isReception = false);

  /**
   * @brief Destructor that stops listening.
   */
  ~IPCManager();

  /**
   * @brief Creates a kitchen channel.
   * @param kitchenId The ID of the kitchen.
   * @throws Exceptions::IPCException if the IPCManager is not a reception.
   */
  void createKitchenChannel(uint32_t kitchenId);

  /**
   * @brief Removes a kitchen channel.
   * @param kitchenId The ID of the kitchen to remove.
   * @throws Exceptions::IPCException if the IPCManager is not a reception.
   */
  void removeKitchenChannel(uint32_t kitchenId);

  /**
   * @brief Sends a message to a specific kitchen.
   * @param kitchenId The ID of the kitchen to send the message to.
   * @param message The message to send.
   * @throws Exceptions::IPCException if the IPCManager is not a reception.
   */
  void sendToKitchen(uint32_t kitchenId, const Message &message);

  /**
   * @brief Broadcasts a message to all kitchens.
   * @param message The message to broadcast.
   * @throws Exceptions::IPCException if the IPCManager is not a reception.
   */
  void broadcastToKitchens(const Message &message);

  /**
   * @brief Connects to the reception.
   * This method is used by kitchens to connect to the reception's inbox.
   * @throws Exceptions::IPCException if the IPCManager is a reception.
   */
  void connectToReception();

  /**
   * @brief Sends a message to the reception.
   * @param message The message to send.
   * @throws Exceptions::IPCException if the IPCManager is a reception or if not
   * connected to the reception.
   */
  void sendToReception(const Message &message);

  /**
   * @brief Sets a message handler for a specific message type.
   * @param type The type of the message to handle.
   * @param handler The function to call when a message of this type is
   * received.
   */
  void setMessageHandler(Message::MessageType type,
                         std::function<void(const Message &)> handler);

  /**
   * @brief Starts listening for messages.
   */
  void startListening();

  /**
   * @brief Stops listening for messages.
   */
  void stopListening();

  /**
   * @brief Checks if the IPCManager is connected to the reception.
   * @return True if connected, false otherwise.
   */
  [[nodiscard]] bool isConnected() const { return m_connected; }

private:
  /**
   * @brief The main loop for listening to messages.
   * This method runs in a separate thread and listens for incoming messages.
   */
  void listenLoop();

  /**
   * @brief Processes a received message.
   * @param message The message to process.
   */
  void processMessage(const Message &message);

  /**
   * @brief Gets the queue name for a message.
   * @param fromId The ID of the sender.
   * @param toId The ID of the receiver.
   * @return The queue name for the message.
   */
  std::string getQueueName(uint32_t fromId, uint32_t toId) const;

  uint32_t m_id;
  bool m_isReception;
  std::atomic<bool> m_connected{false};
  std::atomic<bool> m_listening{false};

  std::unordered_map<uint32_t, std::unique_ptr<MessageQueue>> m_kitchenQueues;
  std::unique_ptr<MessageQueue> m_kitchenInbox;

  std::unique_ptr<MessageQueue> m_receptionInbox;
  std::unique_ptr<MessageQueue> m_receptionOutbox;

  std::unordered_map<Message::MessageType, std::function<void(const Message &)>>
      m_handlers;
  std::thread m_listenerThread;
};
} // namespace Plazza::Communication
