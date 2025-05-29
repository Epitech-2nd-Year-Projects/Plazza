#include "Core/PizzaPacket.hpp"
#include <cstdint>

namespace Plazza::Core {
PizzaPacket::PizzaPacket(const Pizza &pizza) { setPizza(pizza); }

PizzaPacket::PizzaPacket(const OpaqueObject &object) { unpack(object); }

void PizzaPacket::setPizza(const Pizza &pizza) {
  m_type = pizza.getType();
  m_size = pizza.getSize();
}

Pizza PizzaPacket::getPizza() const { return Pizza(m_type, m_size); }

OpaqueObject PizzaPacket::pack() const {
  OpaqueObject object;

  object.pack(static_cast<uint32_t>(m_type));
  object.pack(static_cast<uint32_t>(m_size));
  object.pack(m_orderId);
  object.pack(m_kitchenId);
  return object;
}

void PizzaPacket::unpack(const OpaqueObject &object) {
  OpaqueObject mutableObj = object;
  mutableObj.reset();

  uint32_t typeValue;
  uint32_t sizeValue;

  mutableObj.unpack(typeValue);
  mutableObj.unpack(sizeValue);
  mutableObj.unpack(m_orderId);
  mutableObj.unpack(m_kitchenId);

  m_type = static_cast<PizzaType>(typeValue);
  m_size = static_cast<PizzaSize>(sizeValue);
}

bool PizzaPacket::isValid() const {
  switch (m_type) {
  case PizzaType::Regina:
  case PizzaType::Margarita:
  case PizzaType::Americana:
  case PizzaType::Fantasia:
    break;
  default:
    return false;
  }

  switch (m_size) {
  case PizzaSize::S:
  case PizzaSize::M:
  case PizzaSize::L:
  case PizzaSize::XL:
  case PizzaSize::XXL:
    break;
  default:
    return false;
  }

  return true;
}
} // namespace Plazza::Core
