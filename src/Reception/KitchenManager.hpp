/**
 * @file KitchenManager.hpp
 * @brief Defines the KitchenManager class for managing kitchen operations.
 */

#pragma once

#include "Communication/IPCManager.hpp"
#include "Communication/Serialization.hpp"
#include "Core/Process.hpp"
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Plazza::Reception {
/**
 * @struct KitchenInfo
 * @brief Contains information about a kitchen process.
 */
struct KitchenInfo {
  uint32_t id;
  std::unique_ptr<Core::Process> process;
  std::chrono::steady_clock::time_point lastHeartbeat;
  Communication::KitchenStatus status;
  bool active = true;
};

/**
 * @class KitchenManager
 * @brief Manages kitchen processes and distributes pizza orders.
 */
class KitchenManager {
public:
  /**
   * @brief Constructs a KitchenManager instance.
   * @param cooksPerKitchen Number of cooks per kitchen.
   * @param stockRestockTime Time interval for stock replenishment.
   * @param timeMultiplier Multiplier for cooking time to simulate different
   * cooking speeds.
   */
  KitchenManager(uint32_t cooksPerKitchen,
                 std::chrono::milliseconds stockRestockTime,
                 double timeMultiplier);

  /**
   * @brief Destructor that stops the kitchen manager.
   */
  ~KitchenManager();

  /**
   * @brief Distributes pizza orders to the best available kitchen.
   * @param orders Vector of pizza orders to distribute.
   */
  void distributeOrder(const std::vector<Communication::PizzaOrder> &orders);

  /**
   * @brief Displays the status of all kitchens.
   */
  void displayStatus() const;

  /**
   * @brief Cleans up resources and stops all kitchen processes.
   */
  void cleanup();

private:
  /**
   * @brief Sets up message handlers for kitchen-related messages.
   */
  void setupMessageHandlers();

  /**
   * @brief Handles pizza completion messages.
   * @param message The received message containing pizza completion details.
   */
  void handlePizzaCompleted(const Communication::Message &message);

  /**
   * @brief Handles status response messages from kitchens.
   * @param message The received message containing kitchen status.
   */
  void handleStatusResponse(const Communication::Message &message);

  /**
   * @brief Handles heartbeat messages from kitchens.
   * @param message The received message indicating kitchen activity.
   */
  void handleHeartbeat(const Communication::Message &message);

  /**
   * @brief Finds the best kitchen to handle a new order.
   * @return The ID of the best kitchen, or 0 if no suitable kitchen is found.
   */
  uint32_t findBestKitchen() const;

  /**
   * @brief Creates a new kitchen process.
   */
  void createKitchen();

  /**
   * @brief Removes kitchens that have not sent a heartbeat within the timeout.
   */
  void removeInactiveKitchens();

  /**
   * @brief Requests status updates from all kitchens.
   */
  void requestStatusUpdates();

private:
  static constexpr uint32_t MAX_PIZZAS_PER_KITCHEN_MULTIPLIER = 2;
  static constexpr std::chrono::seconds HEARTBEAT_TIMEOUT{10};

  std::unordered_map<uint32_t, std::unique_ptr<KitchenInfo>> m_kitchens;
  std::unique_ptr<Communication::IPCManager> m_ipcManager;
  uint32_t m_nextKitchenId = 1;
  uint32_t m_cooksPerKitchen;
  std::chrono::milliseconds m_stockRestockTime;
  double m_timeMultiplier;
};
} // namespace Plazza::Reception
