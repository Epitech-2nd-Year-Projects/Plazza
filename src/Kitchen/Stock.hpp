/**
 * @file Stock.hpp
 * @brief Defines the Stock class for managing stock in the kitchen.
 */

#pragma once

#include "Core/Pizza.hpp"
#include "Core/Thread.hpp"
#include <atomic>
#include <chrono>
#include <map>
#include <mutex>

namespace Plazza::Kitchen {
/**
 * @class Stock
 * @brief Manage stock of ingredients in the kitchen.
 */
class Stock {
public:
  /**
   * @brief Constructs a Stock instance.
   * @param restockTime Time to restock the stock.
   */
  Stock(std::chrono::milliseconds restockTime);

  /**
   * @brief Destructor that stops the restock thread.
   */
  ~Stock();

  /**
   * @brief Consumes ingredients from the stock.
   * @param ingredients The ingredients to consume.
   * @return True if all ingredients were available and consumed, false
   * otherwise.
   */
  [[nodiscard]] bool
  consumeIngredients(const std::vector<Core::Ingredient> &ingredients);

  /**
   * @brief Gets the current stock of ingredients.
   * @return A map of ingredients and their quantities.
   */
  [[nodiscard]] std::map<Core::Ingredient, uint32_t> getStock() const;

  /**
   * @brief Starts the restock thread.
   */
  void startRestock();

  /**
   * @brief Stops the restock thread.
   * This method will stop the periodic restocking of ingredients.
   */
  void stopRestock();

private:
  /**
   * @brief The main restocking loop that periodically restocks the stock.
   * This method runs in a separate thread and periodically restocks the stock.
   */
  void restock();

  mutable std::mutex m_mutex;
  std::map<Core::Ingredient, uint32_t> m_stock;
  std::chrono::milliseconds m_restockTime;
  Core::Thread m_thread;
  std::atomic<bool> m_shouldRestock{true};
};
} // namespace Plazza::Kitchen
