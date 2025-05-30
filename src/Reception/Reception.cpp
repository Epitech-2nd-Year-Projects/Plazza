#include "Reception/Reception.hpp"
#include "Logger/Logger.hpp"
#include "Reception/OrderParser.hpp"
#include <iostream>

namespace Plazza::Reception {
Reception::Reception(double timeMultiplier, uint32_t cooksPerKitchen,
                     std::chrono::milliseconds stockRegenTime)
    : m_kitchenManager(std::make_unique<KitchenManager>(
          cooksPerKitchen, stockRegenTime, timeMultiplier)) {}

void Reception::run() {
  std::string input;
  while (m_running) {
    if (!std::getline(std::cin, input)) {
      break;
    }

    if (!input.empty()) {
      processCommand(input);
    }
  }
  m_kitchenManager->cleanup();
}

void Reception::processCommand(const std::string &command) {
  std::string trimmedCommand = command;
  trimmedCommand.erase(0, trimmedCommand.find_first_not_of(" \t"));
  trimmedCommand.erase(trimmedCommand.find_last_not_of(" \t") + 1);

  if (trimmedCommand == "exit" || trimmedCommand == "quit") {
    m_running = false;
    return;
  }

  if (trimmedCommand == "status") {
    m_kitchenManager->displayStatus();
    return;
  }

  try {
    std::vector<Communication::PizzaOrder> orders =
        OrderParser::parseOrder(trimmedCommand);
    if (!orders.empty()) {
      m_kitchenManager->distributeOrder(orders);
      LOG_INFO("Order placed: " + std::to_string(orders.size()) + " pizzas");
    }
  } catch (const std::exception &e) {
    LOG_ERROR(std::string("Error: ") + e.what());
  }
}
} // namespace Plazza::Reception
