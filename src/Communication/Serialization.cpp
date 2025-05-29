#include "Communication/Serialization.hpp"
#include <chrono>

namespace Plazza::Communication {
Core::OpaqueObject PizzaOrder::pack() const {
  Core::OpaqueObject object;

  object.pack(static_cast<uint32_t>(type));
  object.pack(static_cast<uint32_t>(size));
  object.pack(quantity);
  object.pack(orderId);

  return object;
}

void PizzaOrder::unpack(const Core::OpaqueObject &object) {
  Core::OpaqueObject mutableObject = object;
  mutableObject.reset();

  uint32_t typeValue, sizeValue;
  mutableObject.unpack(typeValue);
  mutableObject.unpack(sizeValue);
  mutableObject.unpack(quantity);
  mutableObject.unpack(orderId);

  type = static_cast<Core::PizzaType>(typeValue);
  size = static_cast<Core::PizzaSize>(sizeValue);
}

Core::OpaqueObject KitchenStatus::pack() const {
  Core::OpaqueObject object;

  object.pack(kitchenId);
  object.pack(busyCooks);
  object.pack(totalCooks);
  object.pack(pendingPizzas);
  object.pack(static_cast<uint32_t>(stock.size()));

  for (const auto &[ingredient, count] : stock) {
    object.pack(static_cast<uint32_t>(ingredient));
    object.pack(count);
  }

  return object;
}

void KitchenStatus::unpack(const Core::OpaqueObject &object) {
  Core::OpaqueObject mutableObject = object;
  mutableObject.reset();

  mutableObject.unpack(kitchenId);
  mutableObject.unpack(busyCooks);
  mutableObject.unpack(totalCooks);
  mutableObject.unpack(pendingPizzas);

  uint32_t stockSize;
  mutableObject.unpack(stockSize);
  stock.clear();
  stock.reserve(stockSize);

  for (uint32_t i = 0; i < stockSize; ++i) {
    uint32_t ingredientValue, count;
    mutableObject.unpack(ingredientValue);
    mutableObject.unpack(count);
    stock.emplace_back(static_cast<Core::Ingredient>(ingredientValue), count);
  }
}

Core::OpaqueObject PizzaCompletion::pack() const {
  Core::OpaqueObject object;

  Core::OpaqueObject pizzaObject = pizza.pack();
  object.pack(pizzaObject.getData());

  auto completionTimeNs = completionTime.time_since_epoch();
  auto totalNanoseconds =
      std::chrono::duration_cast<std::chrono::nanoseconds>(completionTimeNs)
          .count();
  object.pack(static_cast<uint64_t>(totalNanoseconds));

  return object;
}

void PizzaCompletion::unpack(const Core::OpaqueObject &object) {
  Core::OpaqueObject mutableObject = object;
  mutableObject.reset();

  std::vector<uint8_t> pizzaData;
  mutableObject.unpack(pizzaData);
  Core::OpaqueObject pizzaObj(std::move(pizzaData));
  pizza.unpack(pizzaObj);

  uint64_t nanosecondsCount;
  mutableObject.unpack(nanosecondsCount);
  completionTime = std::chrono::steady_clock::time_point(
      std::chrono::nanoseconds(nanosecondsCount));
}
} // namespace Plazza::Communication
