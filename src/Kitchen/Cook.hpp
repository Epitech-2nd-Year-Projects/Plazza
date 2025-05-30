/**
 * @file Cook.hpp
 * @brief Defines the Cook class for managing cook threads in the kitchen.
 */

#pragma once

#include "Core/Pizza.hpp"
#include "Core/Thread.hpp"
#include "Core/ThreadQueue.hpp"
#include <atomic>
#include <functional>

namespace Plazza::Kitchen {
/**
 * @class Cook
 * @brief Manages a cook thread that prepares pizzas.
 */
class Cook {
public:
  /**
   * @brief Constructs a Cook instance.
   * @param id Unique identifier for the cook.
   * @param callback Callback function to call when a pizza is completed.
   * @param timeMultiplier Multiplier for cooking time to simulate different
   * cooking speeds.
   */
  Cook(uint32_t id, std::function<void(const Core::Pizza &)> callback,
       double timeMultiplier);

  /**
   * @brief Destructor that stops the cook thread.
   */
  ~Cook();

  /**
   * @brief Starts the cook thread.
   * This method initializes the cooking loop in a separate thread.
   */
  void start();

  /**
   * @brief Stops the cook thread.
   * This method signals the cooking loop to stop and waits for the thread to
   * finish.
   */
  void stop();

  /**
   * @brief Checks if the cook is currently busy preparing a pizza.
   * @return True if the cook is busy, false otherwise.
   */
  [[nodiscard]] bool isBusy() const { return m_isBusy.load(); }

  /**
   * @brief Assigns a pizza to the cook for preparation.
   * @param pizza The pizza to be prepared.
   * @return True if the pizza was successfully assigned, false if the cook is
   * busy.
   */
  bool assignPizza(const Core::Pizza &pizza);

private:
  /**
   * @brief The main cooking loop that processes pizzas from the queue.
   * This method runs in a separate thread and continuously checks for pizzas to
   * cook.
   */
  void cookingLoop();

  /**
   * @brief Cooks a pizza.
   * This method simulates the cooking process for a given pizza.
   * @param pizza The pizza to be cooked.
   */
  void cookPizza(const Core::Pizza &pizza);

  uint32_t m_id;
  std::function<void(const Core::Pizza &)> m_callback;
  double m_timeMultiplier;
  Core::Thread m_thread;
  Core::ThreadQueue<Core::Pizza> m_pizzaQueue;
  std::atomic<bool> m_isBusy{false};
  std::atomic<bool> m_shouldStop{false};
};
} // namespace Plazza::Kitchen
