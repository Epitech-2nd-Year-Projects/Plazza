/**
 * @file Pizza.cpp
 * @brief Implements the Pizza class for managing pizza.
 */

#include "Core/Pizza.hpp"
#include "Exceptions/ArgumentException.hpp"
#include <algorithm>
#include <cstring>

namespace Plazza::Core {

std::unique_ptr<Pizza> Pizza::createPizza(PizzaType type, PizzaSize size) {
  switch (type) {
  case PizzaType::Margarita:
    return std::make_unique<MargaritaPizza>(size);
  case PizzaType::Regina:
    return std::make_unique<ReginaPizza>(size);
  case PizzaType::Americana:
    return std::make_unique<AmericanaPizza>(size);
  case PizzaType::Fantasia:
    return std::make_unique<FantasiaPizza>(size);
  default:
    throw Exceptions::ArgumentException(
        "Pizza::createPizza: Unknown pizza type");
  }
}

Pizza::Pizza(PizzaType type, PizzaSize size)
    : m_type(type), m_size(size), m_baseCookingTime(0) {}

std::string Pizza::getName() const { return toString(m_type); }

std::string Pizza::getSizeName() const { return toString(m_size); }

MargaritaPizza::MargaritaPizza(PizzaSize size)
    : Pizza(PizzaType::Margarita, size) {
  m_ingredients = {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere};
  m_baseCookingTime = 1;
}

ReginaPizza::ReginaPizza(PizzaSize size) : Pizza(PizzaType::Regina, size) {
  m_ingredients = {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere,
                   Ingredient::Ham, Ingredient::Mushrooms};
  m_baseCookingTime = 2;
}

AmericanaPizza::AmericanaPizza(PizzaSize size)
    : Pizza(PizzaType::Americana, size) {
  m_ingredients = {Ingredient::Dough, Ingredient::Tomato, Ingredient::Gruyere,
                   Ingredient::Steak};
  m_baseCookingTime = 2;
}

FantasiaPizza::FantasiaPizza(PizzaSize size)
    : Pizza(PizzaType::Fantasia, size) {
  m_ingredients = {Ingredient::Dough, Ingredient::Tomato, Ingredient::Eggplant,
                   Ingredient::GoatCheese, Ingredient::ChiefLove};
  m_baseCookingTime = 4;
}

std::string toString(PizzaType type) {
  switch (type) {
  case PizzaType::Regina:
    return "regina";
  case PizzaType::Margarita:
    return "margarita";
  case PizzaType::Americana:
    return "americana";
  case PizzaType::Fantasia:
    return "fantasia";
  default:
    return "unknown";
  }
}

std::string toString(PizzaSize size) {
  switch (size) {
  case PizzaSize::S:
    return "S";
  case PizzaSize::M:
    return "M";
  case PizzaSize::L:
    return "L";
  case PizzaSize::XL:
    return "XL";
  case PizzaSize::XXL:
    return "XXL";
  default:
    return "unknown";
  }
}

std::string toString(Ingredient ingredient) {
  switch (ingredient) {
  case Ingredient::Dough:
    return "dough";
  case Ingredient::Tomato:
    return "tomato";
  case Ingredient::Gruyere:
    return "gruyere";
  case Ingredient::Ham:
    return "ham";
  case Ingredient::Mushrooms:
    return "mushrooms";
  case Ingredient::Steak:
    return "steak";
  case Ingredient::Eggplant:
    return "eggplant";
  case Ingredient::GoatCheese:
    return "goat cheese";
  case Ingredient::ChiefLove:
    return "chief love";
  default:
    return "unknown";
  }
}

PizzaType pizzaTypeFromString(const std::string &type) {
  std::string normalizedType = type;
  std::transform(normalizedType.begin(), normalizedType.end(),
                 normalizedType.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (normalizedType == "regina")
    return PizzaType::Regina;
  if (normalizedType == "margarita")
    return PizzaType::Margarita;
  if (normalizedType == "americana")
    return PizzaType::Americana;
  if (normalizedType == "fantasia")
    return PizzaType::Fantasia;

  throw Exceptions::ArgumentException(
      "Pizza::pizzaTypeFromString: Invalid pizza type: " + type);
}

PizzaSize pizzaSizeFromString(const std::string &size) {
  std::string normalizedSize = size;
  std::transform(normalizedSize.begin(), normalizedSize.end(),
                 normalizedSize.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  if (normalizedSize == "S")
    return PizzaSize::S;
  if (normalizedSize == "M")
    return PizzaSize::M;
  if (normalizedSize == "L")
    return PizzaSize::L;
  if (normalizedSize == "XL")
    return PizzaSize::XL;
  if (normalizedSize == "XXL")
    return PizzaSize::XXL;

  throw Exceptions::ArgumentException(
      "Pizza::pizzaSizeFromString: Invalid pizza size: " + size);
}

} // namespace Plazza::Core
