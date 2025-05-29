/**
 * @file Kitchen.hpp
 * @brief Defines the Kitchen class for managing kitchen operations.
 */

#pragma once

#include "Communication/IPCManager.hpp"
#include "Kitchen/Cook.hpp"
#include "Kitchen/Stock.hpp"
#include <atomic>
#include <chrono>
#include <memory>
#include <vector>

namespace Plazza::Kitchen {
/**
 * @class Kitchen
 * @brief Manages a kitchen with multiple cooks.
 */
class Kitchen {
public:
  /**
   * @brief Constructs a Kitchen instance.
   * @param id Unique identifier for the kitchen.
   * @param cookCount Number of cooks in the kitchen.
   * @param restockInterval Interval for stock replenishment.
   * @param timeMultiplier Multiplier for cooking time to simulate different
   * cooking speeds.
   */
  Kitchen(uint32_t id, uint32_t cookCount,
          std::chrono::milliseconds restockInterval, double timeMultiplier);

  /**
   * @brief Destructor that stops the kitchen.
   */
  ~Kitchen();

  /**
   * @brief Starts the kitchen operations.
   * This method connects to the reception, starts stock replenishment, and
   * initializes cooks.
   */
  void run();

  /**
   * @brief Stops the kitchen operations.
   * This method stops all cooks, stock replenishment, and IPC communication.
   */
  void stop();

private:
  /**
   * @brief Sets up message handlers for the kitchen.
   */
  void setupMessageHandlers();

  /**
   * @brief Handles pizza order messages.
   * @param message The received message containing the pizza order.
   */
  void handlePizzaOrder(const Communication::Message &message);

  /**
   * @brief Handles status request messages.
   * @param message The received message requesting the kitchen status.
   */
  void handleStatusRequest(const Communication::Message &message);

  /**
   * @brief Handles shutdown messages.
   * @param message The received message indicating a shutdown request.
   */
  void handleShutdown(const Communication::Message &message);

  /**
   * @brief Callback function called when a pizza is completed.
   * @param pizza The completed pizza.
   */
  void onPizzaCompleted(const Core::Pizza &pizza);

  /**
   * @brief Sends a heartbeat message to the reception.
   */
  void sendHeartbeat();

  /**
   * @brief Sends the current kitchen status to the reception.
   */
  void sendStatus();

private:
  static constexpr std::chrono::seconds TIMEOUT{5};
  static constexpr std::chrono::seconds HEARTBEAT_INTERVAL{1};

  uint32_t m_id;
  uint32_t m_cooksCount;
  double m_timeMultiplier;
  std::vector<std::unique_ptr<Cook>> m_cooks;
  std::unique_ptr<Stock> m_stock;
  std::unique_ptr<Communication::IPCManager> m_ipcManager;

  std::atomic<uint32_t> m_pendingPizzas{0};
  std::atomic<bool> m_running{true};
  std::chrono::steady_clock::time_point m_lastActivity;
};
} // namespace Plazza::Kitchen
