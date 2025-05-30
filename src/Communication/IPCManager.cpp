#include "Communication/IPCManager.hpp"
#include "Exceptions/IPCException.hpp"
#include "Logger/Logger.hpp"
#include <chrono>

namespace Plazza::Communication {

IPCManager::IPCManager(uint32_t id, bool isReception, uint32_t cooksCount)
    : m_id(id), m_isReception(isReception), m_cooksCount(cooksCount) {

  if (m_isReception) {
    m_receptionInbox =
        std::make_unique<MessageQueue>("reception_inbox", true, cooksCount);
  }
}

IPCManager::~IPCManager() { stopListening(); }

void IPCManager::createKitchenChannel(uint32_t kitchenId) {
  if (!m_isReception) {
    throw Exceptions::IPCException(
        "Only reception can create kitchen channels");
  }

  std::string queueName = "kitchen_" + std::to_string(kitchenId) + "_inbox";
  m_kitchenQueues[kitchenId] =
      std::make_unique<MessageQueue>(queueName, true, m_cooksCount);
}

void IPCManager::removeKitchenChannel(uint32_t kitchenId) {
  if (!m_isReception) {
    throw Exceptions::IPCException(
        "Only reception can remove kitchen channels");
  }

  m_kitchenQueues.erase(kitchenId);
}

void IPCManager::sendToKitchen(uint32_t kitchenId, const Message &message) {
  if (!m_isReception) {
    throw Exceptions::IPCException("Only reception can send to kitchens");
  }

  auto it = m_kitchenQueues.find(kitchenId);
  if (it != m_kitchenQueues.end()) {
    it->second->send(message.serialize());
  }
}

void IPCManager::broadcastToKitchens(const Message &message) {
  if (!m_isReception) {
    throw Exceptions::IPCException(
        "Broadcasting to kitchens is only allowed from reception");
  }

  for (const auto &[kitchenId, queue] : m_kitchenQueues) {
    try {
      queue->send(message.serialize());
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to send message to kitchen " +
                std::to_string(kitchenId) + ": " + e.what());
    }
  }
}

void IPCManager::connectToReception() {
  if (m_isReception) {
    throw Exceptions::IPCException("Reception doesn't connect to itself");
  }

  std::string inboxName = "kitchen_" + std::to_string(m_id) + "_inbox";
  m_kitchenInbox =
      std::make_unique<MessageQueue>(inboxName, false, m_cooksCount);

  m_receptionOutbox =
      std::make_unique<MessageQueue>("reception_inbox", false, m_cooksCount);
  m_connected = true;
}

void IPCManager::sendToReception(const Message &message) {
  if (m_isReception) {
    throw Exceptions::IPCException("Reception doesn't send to itself");
  }

  if (!m_connected || !m_receptionOutbox) {
    throw Exceptions::IPCException("Not connected to reception");
  }

  m_receptionOutbox->send(message.serialize());
}

void IPCManager::setMessageHandler(
    Message::MessageType type, std::function<void(const Message &)> handler) {
  m_handlers[type] = std::move(handler);
}

void IPCManager::startListening() {
  if (m_listening) {
    return;
  }

  m_listening = true;
  m_listenerThread = std::thread(&IPCManager::listenLoop, this);
}

void IPCManager::stopListening() {
  if (m_listening) {
    m_listening = false;
    if (m_listenerThread.joinable()) {
      m_listenerThread.join();
    }
  }
}

void IPCManager::listenLoop() {
  MessageQueue *inbox =
      m_isReception ? m_receptionInbox.get() : m_kitchenInbox.get();

  if (!inbox) {
    return;
  }

  while (m_listening) {
    try {
      std::optional<std::string> messageData =
          inbox->timedReceive(std::chrono::milliseconds(100));
      if (messageData) {
        Message message = Message::deserialize(*messageData);
        processMessage(message);
      }
    } catch (const std::exception &e) {
      if (m_listening) {
        LOG_ERROR("Error receiving message: " + std::string(e.what()));
      }
    }
  }
}

void IPCManager::processMessage(const Message &message) {
  auto it = m_handlers.find(message.getType());
  if (it != m_handlers.end()) {
    try {
      it->second(message);
    } catch (const std::exception &e) {
      LOG_ERROR("Error processing message: " + std::string(e.what()));
    }
  }
}
} // namespace Plazza::Communication
