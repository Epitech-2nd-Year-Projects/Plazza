/**
 * @file Reception.hpp
 * @brief Defines the Reception class for running the reception process.
 */

#pragma once

#include "Reception/KitchenManager.hpp"
#include <memory>
#include <string>

namespace Plazza::Reception {
/**
 * @class Reception
 * @brief Manages the reception process.
 */
class Reception {
public:
  /**
   * @brief Constructs a Reception instance.
   * @param timeMultiplier Multiplier for cooking time to simulate different
   * cooking speeds.
   * @param cooksPerKitchen Number of cooks per kitchen.
   * @param stockRestockTime Time interval for stock replenishment.
   */
  Reception(double timeMultiplier, uint32_t cooksPerKitchen,
            std::chrono::milliseconds stockRestockTime);

  /**
   * @brief Runs the reception process, handling user input and distributing
   * orders.
   */
  void run();

private:
  /**
   * @brief Processes a command entered by the user.
   * @param command The command string to process.
   */
  void processCommand(const std::string &command);

private:
  std::unique_ptr<KitchenManager> m_kitchenManager;
  bool m_running = true;
};
} // namespace Plazza::Reception
