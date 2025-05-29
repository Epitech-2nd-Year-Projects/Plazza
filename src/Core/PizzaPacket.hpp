/**
 * @file PizzaPacket.hpp
 * @brief Defines the PizzaPacket class for representing pizza orders.
 */

#pragma once

#include "Core/OpaqueObject.hpp"
#include "Core/Pizza.hpp"

namespace Plazza::Core {
/**
 * @class PizzaPacket
 * @brief A class for representing pizza orders in a packet format.
 */
class PizzaPacket {
public:
  /**
   * @brief Default constructor.
   */
  PizzaPacket() = default;

  /**
   * @brief Constructs a PizzaPacket from a Pizza object.
   * @param pizza The pizza to pack into the packet.
   */
  explicit PizzaPacket(const Pizza &pizza);

  /**
   * @brief Constructs a PizzaPacket from an OpaqueObject.
   * @param object The OpaqueObject to unpack into the packet.
   */
  explicit PizzaPacket(const OpaqueObject &object);

  /**
   * @brief Sets the pizza in the packet.
   * @param pizza The pizza to set.
   */
  void setPizza(const Pizza &pizza);

  /**
   * @brief Gets the pizza from the packet.
   * @return The pizza contained in the packet.
   */
  Pizza getPizza() const;

  /**
   * @brief Sets the order ID.
   * @param orderId The ID of the pizza order.
   */
  void setOrderId(uint32_t orderId) { m_orderId = orderId; }

  /**
   * @brief Gets the order ID.
   * @return The order ID of the pizza packet.
   */
  [[nodiscard]] uint32_t getOrderId() const { return m_orderId; }

  /**
   * @brief Sets the kitchen ID.
   * @param kitchenId The ID of the kitchen processing the order.
   */
  void setKitchenId(uint32_t kitchenId) { m_kitchenId = kitchenId; }

  /**
   * @brief Gets the kitchen ID.
   * @return The kitchen ID of the pizza packet.
   */
  [[nodiscard]] uint32_t getKitchenId() const { return m_kitchenId; }

  /**
   * @brief Packs the pizza packet into an OpaqueObject.
   * @return An OpaqueObject containing the packed pizza packet data.
   */
  OpaqueObject pack() const;

  /**
   * @brief Unpacks the pizza packet from an OpaqueObject.
   * @param object The OpaqueObject containing the packed pizza packet data.
   */
  void unpack(const OpaqueObject &object);

  /**
   * @brief Overloaded operator to unpack the pizza packet into an OpaqueObject.
   * @param object The OpaqueObject to unpack into.
   * @return The OpaqueObject containing the packed pizza packet data.
   */
  PizzaPacket &operator<<(const OpaqueObject &object) {
    unpack(object);
    return *this;
  }

  /**
   * @brief Overloaded operator to pack the pizza packet into an OpaqueObject.
   * @param object The OpaqueObject to pack into.
   * @return Reference to this PizzaPacket object.
   */
  OpaqueObject operator>>(OpaqueObject &object) const {
    object = pack();
    return object;
  }

  /**
   * @brief Checks if the pizza packet is valid.
   * @return True if the pizza packet is valid, false otherwise.
   */
  [[nodiscard]] bool isValid() const;

private:
  PizzaType m_type = PizzaType::Margarita;
  PizzaSize m_size = PizzaSize::S;
  uint32_t m_orderId = 0;
  uint32_t m_kitchenId = 0;
};
} // namespace Plazza::Core
