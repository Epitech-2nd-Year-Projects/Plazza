/**
 * @file Serialization.hpp
 * @brief Defines the Serialization class for serializing and deserializing
 * data.
 */

#pragma once

#include "Core/OpaqueObject.hpp"
#include "Core/Pizza.hpp"
#include "Core/PizzaPacket.hpp"
#include <chrono>
#include <vector>

namespace Plazza::Communication {
/**
 * @struct PizzaOrder
 * @brief A struct representing a pizza order.
 */
struct PizzaOrder {
  Core::PizzaType type;
  Core::PizzaSize size;
  uint32_t quantity;
  uint32_t orderId;

  Core::OpaqueObject pack() const;
  void unpack(const Core::OpaqueObject &object);
};

/**
 * @struct KitchenStatus
 * @brief A struct representing the status of a kitchen.
 */
struct KitchenStatus {
  uint32_t kitchenId;
  uint32_t busyCooks;
  uint32_t totalCooks;
  uint32_t pendingPizzas;
  std::vector<std::pair<Core::Ingredient, uint32_t>> stock;

  Core::OpaqueObject pack() const;
  void unpack(const Core::OpaqueObject &object);
};

/**
 * @struct PizzaCompletion
 * @brief A struct representing a completed pizza.
 */
struct PizzaCompletion {
  Core::PizzaPacket pizza;
  std::chrono::steady_clock::time_point completionTime;

  Core::OpaqueObject pack() const;
  void unpack(const Core::OpaqueObject &object);
};
} // namespace Plazza::Communication
