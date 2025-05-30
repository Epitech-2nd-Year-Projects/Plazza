#include "Reception/Reception.hpp"
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
      std::cout << "Order placed: " << orders.size() << " pizzas" << std::endl;
    }
  } catch (const std::exception &e) {
    std::cout << "Error: " << e.what() << std::endl;
  }
}
} // namespace Plazza::Reception
