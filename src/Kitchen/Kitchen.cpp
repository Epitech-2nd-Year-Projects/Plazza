#include "Kitchen/Kitchen.hpp"
#include "Communication/Serialization.hpp"
#include "Core/Pizza.hpp"
#include "Core/PizzaPacket.hpp"
#include <iostream>
#include <thread>

namespace Plazza::Kitchen {
Kitchen::Kitchen(uint32_t id, uint32_t cookCount,
                 std::chrono::milliseconds restockInterval,
                 double timeMultiplier)
    : m_id(id), m_cooksCount(cookCount), m_timeMultiplier(timeMultiplier) {
  m_stock = std::make_unique<Stock>(restockInterval);

  m_cooks.reserve(m_cooksCount);
  for (uint32_t i = 0; i < m_cooksCount; ++i) {
    std::unique_ptr<Cook> cook = std::make_unique<Cook>(
        i + 1, [this](const Core::Pizza &pizza) { onPizzaCompleted(pizza); },
        m_timeMultiplier);
    m_cooks.push_back(std::move(cook));
  }

  m_ipcManager =
      std::make_unique<Communication::IPCManager>(m_id, false, m_cooksCount);
  setupMessageHandlers();
  m_lastActivity = std::chrono::steady_clock::now();
}

Kitchen::~Kitchen() { stop(); }

void Kitchen::run() {
  try {
    m_ipcManager->connectToReception();
    m_stock->startRestock();

    for (auto &cook : m_cooks) {
      cook->start();
    }

    m_ipcManager->startListening();

    std::cout << "Kitchen " << m_id << " started with " << m_cooksCount
              << " cooks" << std::endl;

    auto lastHeartbeat = std::chrono::steady_clock::now();

    while (m_running) {
      auto now = std::chrono::steady_clock::now();

      if (now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        lastHeartbeat = now;
      }

      processPendingOrders();

      if (now - m_lastActivity >= TIMEOUT) {
        std::cout << "Kitchen " << m_id << " timed out due to inactivity"
                  << std::endl;
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

  } catch (const std::exception &e) {
    std::cerr << "Kitchen " << m_id << " error: " << e.what() << std::endl;
  }

  std::cout << "Kitchen " << m_id << " shutting down" << std::endl;
}

void Kitchen::stop() {
  m_running = false;

  if (m_ipcManager) {
    m_ipcManager->stopListening();
  }

  for (auto &cook : m_cooks) {
    cook->stop();
  }

  if (m_stock) {
    m_stock->stopRestock();
  }
}

void Kitchen::setupMessageHandlers() {
  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::PIZZA_ORDER,
      [this](const Communication::Message &message) {
        handlePizzaOrder(message);
      });

  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::STATUS_REQUEST,
      [this](const Communication::Message &message) {
        handleStatusRequest(message);
      });

  m_ipcManager->setMessageHandler(
      Communication::Message::MessageType::SHUTDOWN,
      [this](const Communication::Message &message) {
        handleShutdown(message);
      });
}

void Kitchen::handlePizzaOrder(const Communication::Message &message) {
  try {
    Core::OpaqueObject object =
        Core::OpaqueObject::fromString(message.getPayload());

    Communication::PizzaOrder order;
    order.unpack(object);

    std::unique_ptr<Core::Pizza> pizza =
        Core::Pizza::createPizza(order.type, order.size);
    auto &ingredients = pizza->getIngredients();

    bool assigned = m_stock->consumeIngredients(ingredients, [&]() -> bool {
      for (auto &cook : m_cooks) {
        if (cook->assignPizza(*pizza)) {
          ++m_pendingPizzas;
          m_lastActivity = std::chrono::steady_clock::now();
          std::cout << "Kitchen " << m_id << " accepted pizza order: "
                    << Core::toString(pizza->getType()) << " "
                    << Core::toString(pizza->getSize()) << std::endl;
          return true;
        }
      }
      return false;
    });

    if (!assigned) {
      std::lock_guard<std::mutex> lockGuard(m_pendingMutex);
      m_pendingOrders.emplace_back(order);
      std::cout << "Kitchen " << m_id
                << " queued pizza order (no cook/stock available): "
                << Core::toString(order.type) << " "
                << Core::toString(order.size) << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error handling pizza order in kitchen " << m_id << ": "
              << e.what() << std::endl;
  }
}

void Kitchen::handleStatusRequest(
    [[maybe_unused]] const Communication::Message &message) {
  sendStatus();
  m_lastActivity = std::chrono::steady_clock::now();
}

void Kitchen::handleShutdown(
    [[maybe_unused]] const Communication::Message &message) {
  std::cout << "Kitchen " << m_id << " received shutdown signal" << std::endl;
  m_running = false;
}

void Kitchen::onPizzaCompleted(const Core::Pizza &pizza) {
  Communication::PizzaCompletion completion;
  completion.pizza.setPizza(pizza);
  completion.pizza.setKitchenId(m_id);
  completion.completionTime = std::chrono::steady_clock::now();

  Core::OpaqueObject object = completion.pack();

  Communication::Message message{
      Communication::Message::MessageType::PIZZA_COMPLETED, m_id,
      static_cast<uint32_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count()),
      object.toString()};

  m_ipcManager->sendToReception(message);
  m_pendingPizzas--;
  m_lastActivity = std::chrono::steady_clock::now();
}

void Kitchen::sendHeartbeat() {
  Communication::Message message{
      Communication::Message::MessageType::HEARTBEAT, m_id,
      static_cast<uint32_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count()),
      ""};

  try {
    m_ipcManager->sendToReception(message);
  } catch (const std::exception &e) {
    std::cerr << "Kitchen " << m_id << " failed to send heartbeat: " << e.what()
              << std::endl;
  }
}

void Kitchen::sendStatus() {
  uint32_t busyCooks = 0;
  for (const auto &cook : m_cooks) {
    if (cook->isBusy()) {
      busyCooks++;
    }
  }

  Communication::KitchenStatus status;
  status.kitchenId = m_id;
  status.busyCooks = busyCooks;
  status.totalCooks = m_cooksCount;
  status.pendingPizzas = m_pendingPizzas;

  std::map<Core::Ingredient, uint32_t> stockMap = m_stock->getStock();
  status.stock.clear();
  for (const auto &[ingredient, count] : stockMap) {
    status.stock.emplace_back(ingredient, count);
  }

  Core::OpaqueObject object = status.pack();

  Communication::Message message{
      Communication::Message::MessageType::STATUS_RESPONSE, m_id,
      static_cast<uint32_t>(
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch())
              .count()),
      object.toString()};

  try {
    m_ipcManager->sendToReception(message);
  } catch (const std::exception &e) {
    std::cerr << "Kitchen " << m_id << " failed to send status: " << e.what()
              << std::endl;
  }
}

void Kitchen::processPendingOrders() {
  std::lock_guard<std::mutex> lg(m_pendingMutex);
  auto it = m_pendingOrders.begin();
  while (it != m_pendingOrders.end()) {
    auto &order = *it;
    auto pizza = Core::Pizza::createPizza(order.type, order.size);
    auto &ingredients = pizza->getIngredients();

    bool assigned = m_stock->consumeIngredients(ingredients, [&]() -> bool {
      for (auto &cook : m_cooks) {
        if (cook->assignPizza(*pizza)) {
          ++m_pendingPizzas;
          m_lastActivity = std::chrono::steady_clock::now();
          std::cout << "Kitchen " << m_id << " assigned pending pizza order: "
                    << Core::toString(pizza->getType()) << " "
                    << Core::toString(pizza->getSize()) << std::endl;
          return true;
        }
      }
      return false;
    });

    if (assigned) {
      it = m_pendingOrders.erase(it);
    } else {
      ++it;
    }
  }
}
} // namespace Plazza::Kitchen
