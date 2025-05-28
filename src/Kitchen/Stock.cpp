#include "Kitchen/Stock.hpp"
#include <chrono>

namespace Plazza::Kitchen {
Stock::Stock(std::chrono::milliseconds restockTime)
    : m_restockTime(restockTime) {
  m_stock[Core::Ingredient::Dough] = 5;
  m_stock[Core::Ingredient::Tomato] = 5;
  m_stock[Core::Ingredient::Gruyere] = 5;
  m_stock[Core::Ingredient::Ham] = 5;
  m_stock[Core::Ingredient::Mushrooms] = 5;
  m_stock[Core::Ingredient::Steak] = 5;
  m_stock[Core::Ingredient::Eggplant] = 5;
  m_stock[Core::Ingredient::GoatCheese] = 5;
  m_stock[Core::Ingredient::ChiefLove] = 5;
}

Stock::~Stock() { stopRestock(); }

bool Stock::consumeIngredients(
    const std::vector<Core::Ingredient> &ingredients) {
  std::lock_guard<std::mutex> lock(m_mutex);

  for (const auto &ingredient : ingredients) {
    if (m_stock[ingredient] == 0) {
      return false;
    }
  }

  for (const auto &ingredient : ingredients) {
    m_stock[ingredient]--;
  }

  return true;
}

std::map<Core::Ingredient, uint32_t> Stock::getStock() const {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_stock;
}

void Stock::startRestock() {
  m_shouldRestock = true;
  m_thread.start([this]() { restock(); });
}

void Stock::stopRestock() {
  m_shouldRestock = false;
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

void Stock::restock() {
  while (m_shouldRestock) {
    std::this_thread::sleep_for(m_restockTime);

    if (!m_shouldRestock)
      break;

    {
      std::lock_guard<std::mutex> lock(m_mutex);
      for (auto &[ingredient, count] : m_stock) {
        count++;
      }
    }
  }
}
} // namespace Plazza::Kitchen
