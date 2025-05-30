#include "Reception/KitchenManager.hpp"
#include "Communication/Serialization.hpp"
#include "Core/Pizza.hpp"
#include "Kitchen/Kitchen.hpp"
#include "Logger/Logger.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace Plazza::Reception {
KitchenManager::KitchenManager(uint32_t cooksPerKitchen,
                               std::chrono::milliseconds stockRestockTime,
                               double timeMultiplier)
    : m_cooksPerKitchen(cooksPerKitchen), m_stockRestockTime(stockRestockTime),
      m_timeMultiplier(timeMultiplier) {
  m_ipcManager = std::make_unique<Communication::IPCManager>(
      0, true, cooksPerKitchen * MAX_PIZZAS_PER_KITCHEN_MULTIPLIER);

  setupMessageHandlers();
  m_ipcManager->startListening();
}

KitchenManager::~KitchenManager() { cleanup(); }

void KitchenManager::setupMessageHandlers() {
  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::PIZZA_COMPLETED,
      [this](const Communication::Message &msg) { handlePizzaCompleted(msg); });

  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::STATUS_RESPONSE,
      [this](const Communication::Message &msg) { handleStatusResponse(msg); });

  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::HEARTBEAT,
      [this](const Communication::Message &msg) { handleHeartbeat(msg); });
}

void KitchenManager::distributeOrder(
    const std::vector<Communication::PizzaOrder> &orders) {
  removeInactiveKitchens();

  for (const auto &order : orders) {
    uint32_t kitchenId = findBestKitchen();

    if (kitchenId == 0) {
      createKitchen();
      kitchenId = m_nextKitchenId - 1;
    }

    Core::OpaqueObject object = order.pack();

    Communication::Message message{
        Communication::Message::MessageType::PIZZA_ORDER, 0,
        static_cast<uint32_t>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count()),
        object.toString()};

    try {
      m_ipcManager->sendToKitchen(kitchenId, message);

      auto it = m_kitchens.find(kitchenId);
      if (it != m_kitchens.end()) {
        it->second->status.pendingPizzas++;
        it->second->lastHeartbeat = std::chrono::steady_clock::now();
      }

      LOG_INFO("Assigned pizza " + Core::toString(order.type) + " " +
               Core::toString(order.size) + " to kitchen " +
               std::to_string(kitchenId));
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to send order to kitchen " + std::to_string(kitchenId) +
                ": " + e.what());
    }
  }
  removeInactiveKitchens();
}

void KitchenManager::displayStatus() const {
  std::cout << "\n=== Kitchen Status ===" << std::endl;
  std::cout << std::left << std::setw(10) << "Kitchen" << std::setw(12)
            << "Busy/Total" << std::setw(10) << "Pending" << std::setw(8)
            << "Status" << std::endl;
  std::cout << std::string(50, '-') << std::endl;

  for (const auto &[id, kitchen] : m_kitchens) {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceHeartbeat = now - kitchen->lastHeartbeat;
    bool isActive = timeSinceHeartbeat < HEARTBEAT_TIMEOUT;

    std::cout << std::left << std::setw(10) << id << std::setw(12)
              << (std::to_string(kitchen->status.busyCooks) + "/" +
                  std::to_string(kitchen->status.totalCooks))
              << std::setw(10) << kitchen->status.pendingPizzas << std::setw(8)
              << (isActive ? "Active" : "Inactive") << std::endl;
  }

  if (m_kitchens.empty()) {
    std::cout << "No kitchens running" << std::endl;
  }

  std::cout << "======================" << std::endl;

  const_cast<KitchenManager *>(this)->requestStatusUpdates();
}

void KitchenManager::cleanup() {
  Communication::Message shutdownMessage{
      Communication::Message::MessageType::SHUTDOWN, 0,
      static_cast<uint32_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count()),
      ""};

  for (const auto &[id, kitchen] : m_kitchens) {
    try {
      m_ipcManager->sendToKitchen(id, shutdownMessage);
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to send shutdown to kitchen " + std::to_string(id) +
                ": " + e.what());
    }
  }

  for (auto &[id, kitchen] : m_kitchens) {
    if (kitchen->process) {
      kitchen->process->wait();
    }
  }

  m_kitchens.clear();

  if (m_ipcManager) {
    m_ipcManager->stopListening();
  }
}

uint32_t KitchenManager::findBestKitchen() const {
  uint32_t bestKitchen = 0;
  uint32_t minimumLoadThreshold = UINT32_MAX;

  for (const auto &[id, kitchen] : m_kitchens) {
    if (!kitchen->active)
      continue;

    auto now = std::chrono::steady_clock::now();
    if (now - kitchen->lastHeartbeat > HEARTBEAT_TIMEOUT) {
      continue;
    }

    uint32_t maxCapacity =
        kitchen->status.totalCooks * MAX_PIZZAS_PER_KITCHEN_MULTIPLIER;
    uint32_t currentLoad = kitchen->status.pendingPizzas;

    if (currentLoad < maxCapacity && currentLoad < minimumLoadThreshold) {
      minimumLoadThreshold = currentLoad;
      bestKitchen = id;
    }
  }

  return bestKitchen;
}

void KitchenManager::createKitchen() {
  uint32_t kitchenId = m_nextKitchenId++;

  auto kitchenInfo = std::make_unique<KitchenInfo>();
  kitchenInfo->id = kitchenId;
  kitchenInfo->process = std::make_unique<Core::Process>();
  kitchenInfo->lastHeartbeat = std::chrono::steady_clock::now();
  kitchenInfo->status.kitchenId = kitchenId;
  kitchenInfo->status.totalCooks = m_cooksPerKitchen;
  kitchenInfo->status.busyCooks = 0;
  kitchenInfo->status.pendingPizzas = 0;

  m_ipcManager->createKitchenChannel(kitchenId);

  try {
    kitchenInfo->process->fork([this, kitchenId]() {
      Kitchen::Kitchen kitchen(kitchenId, m_cooksPerKitchen, m_stockRestockTime,
                               m_timeMultiplier);
      kitchen.run();
    });

    m_kitchens[kitchenId] = std::move(kitchenInfo);
    LOG_INFO("Created kitchen " + std::to_string(kitchenId));

  } catch (const std::exception &e) {
    LOG_ERROR("Failed to create kitchen " + std::to_string(kitchenId) + ": " +
              e.what());
    m_ipcManager->removeKitchenChannel(kitchenId);
  }
}

void KitchenManager::removeInactiveKitchens() {
  auto now = std::chrono::steady_clock::now();
  std::vector<uint32_t> toRemove;

  for (const auto &[id, kitchen] : m_kitchens) {
    if (!kitchen->process->isRunning() ||
        (now - kitchen->lastHeartbeat > HEARTBEAT_TIMEOUT)) {
      toRemove.push_back(id);
    }
  }

  for (uint32_t id : toRemove) {
    std::cout << "Removing inactive kitchen " << id << std::endl;
    m_ipcManager->removeKitchenChannel(id);
    m_kitchens.erase(id);
  }
}

void KitchenManager::requestStatusUpdates() {
  Communication::Message message{
      Communication::Message::MessageType::STATUS_REQUEST, 0,
      static_cast<uint32_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count()),
      ""};

  for (const auto &[id, kitchen] : m_kitchens) {
    try {
      m_ipcManager->sendToKitchen(id, message);
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to request status from kitchen " + std::to_string(id) +
                ": " + e.what());
    }
  }
}

void KitchenManager::handlePizzaCompleted(
    const Communication::Message &message) {
  try {
    Core::OpaqueObject object =
        Core::OpaqueObject::fromString(message.getPayload());
    Communication::PizzaCompletion completion;
    completion.unpack(object);

    Core::Pizza pizza = completion.pizza.getPizza();

    LOG_INFO("Pizza completed: " + Core::toString(pizza.getType()) + " " +
             Core::toString(pizza.getSize()) + " from kitchen " +
             std::to_string(completion.pizza.getKitchenId()));

    auto it = m_kitchens.find(message.getSenderId());
    if (it != m_kitchens.end()) {
      it->second->status.pendingPizzas =
          std::max(0u, it->second->status.pendingPizzas - 1);
      it->second->lastHeartbeat = std::chrono::steady_clock::now();
    }

  } catch (const std::exception &e) {
    LOG_ERROR("Error handling pizza completion: " + std::string(e.what()));
  }
}

void KitchenManager::handleStatusResponse(
    const Communication::Message &message) {
  try {
    Core::OpaqueObject object =
        Core::OpaqueObject::fromString(message.getPayload());
    Communication::KitchenStatus status;
    status.unpack(object);

    auto it = m_kitchens.find(message.getSenderId());
    if (it != m_kitchens.end()) {
      it->second->status = status;
      it->second->lastHeartbeat = std::chrono::steady_clock::now();
    }

  } catch (const std::exception &e) {
    LOG_ERROR("Error handling status response: " + std::string(e.what()));
  }
}

void KitchenManager::handleHeartbeat(const Communication::Message &message) {
  auto it = m_kitchens.find(message.getSenderId());
  if (it != m_kitchens.end()) {
    it->second->lastHeartbeat = std::chrono::steady_clock::now();
  }
}
} // namespace Plazza::Reception
