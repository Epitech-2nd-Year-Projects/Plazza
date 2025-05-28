#include "Kitchen/Cook.hpp"
#include "Core/Pizza.hpp"
#include <thread>

namespace Plazza::Kitchen {
Cook::Cook(uint32_t id, std::function<void(const Core::Pizza &)> callback,
           double timeMultiplier)
    : m_id(id), m_callback(std::move(callback)),
      m_timeMultiplier(timeMultiplier) {}

Cook::~Cook() { stop(); }

void Cook::start() {
  if (m_thread.joinable()) {
    return;
  }
  m_shouldStop = false;
  m_thread.start([this]() { cookingLoop(); });
}

void Cook::stop() {
  m_shouldStop = true;
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

bool Cook::assignPizza(const Core::Pizza &pizza) {
  if (m_isBusy) {
    return false;
  }
  m_pizzaQueue.push(pizza);
  return true;
}

void Cook::cookingLoop() {
  while (!m_shouldStop) {
    auto pizzaToCook = m_pizzaQueue.tryPop();
    if (pizzaToCook) {
      m_isBusy = true;
      cookPizza(*pizzaToCook);
      m_isBusy = false;
    } else {
      std::this_thread::yield();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void Cook::cookPizza(const Core::Pizza &pizza) {
  double cookingTime = pizza.getCookingTime(m_timeMultiplier);
  std::chrono::milliseconds adjustedTime =
      std::chrono::milliseconds(static_cast<int>(cookingTime * 1000));

  std::chrono::milliseconds sleepChunk = std::chrono::milliseconds(100);
  std::chrono::milliseconds remainingTime = adjustedTime;

  while (remainingTime > std::chrono::milliseconds(0) && !m_shouldStop) {
    auto sleepTime = std::min(sleepChunk, remainingTime);
    std::this_thread::sleep_for(sleepTime);
    remainingTime -= sleepTime;
  }

  if (!m_shouldStop) {
    if (m_callback) {
      Core::Pizza completedPizza = pizza;
      m_callback(completedPizza);
    }
  }
}
} // namespace Plazza::Kitchen
