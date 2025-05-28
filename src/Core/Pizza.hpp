/**
 * @file Pizza.hpp
 * @brief Defines the Pizza class and its derived classes.
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Plazza::Core {

/**
 * @enum PizzaType
 * @brief Enum representing different types of pizzas.
 */
enum class PizzaType { Regina = 1, Margarita = 2, Americana = 4, Fantasia = 8 };

/**
 * @enum PizzaSize
 * @brief Enum representing different sizes of pizzas.
 */
enum class PizzaSize { S = 1, M = 2, L = 4, XL = 8, XXL = 16 };

/**
 * @enum Ingredient
 * @brief Enum representing different ingredients of pizzas.
 */
enum class Ingredient {
  Dough,
  Tomato,
  Gruyere,
  Ham,
  Mushrooms,
  Steak,
  Eggplant,
  GoatCheese,
  ChiefLove
};

/**
 * @class Pizza
 * @brief Base class for all pizza types.
 */
class Pizza {
public:
  /**
   * @brief Constructor for Pizza.
   * @param type Type of the pizza.
   * @param size Size of the pizza.
   */
  Pizza(PizzaType type, PizzaSize size);

  /**
   * @brief Default destructor for Pizza.
   */
  virtual ~Pizza() = default;

  /**
   * @brief Factory method to create a pizza of the specified type and size.
   * @param type Type of the pizza.
   * @param size Size of the pizza.
   * @return A unique pointer to the created pizza.
   * @throws ArgumentException if the type is not valid.
   */
  static std::unique_ptr<Pizza> createPizza(PizzaType type, PizzaSize size);

  /**
   * @brief Get the type of the pizza.
   * @return The type of the pizza.
   */
  PizzaType getType() const { return m_type; }

  /**
   * @brief Get the size of the pizza.
   * @return The size of the pizza.
   */
  PizzaSize getSize() const { return m_size; }

  /**
   * @brief Get the cooking time of the pizza.
   * @param multiplier Multiplier for the cooking time.
   * @return The cooking time of the pizza.
   */
  double getCookingTime(double multiplier) const {
    return m_baseCookingTime * multiplier;
  }

  /**
   * @brief Get the ingredients of the pizza.
   * @return A vector of ingredients.
   */
  const std::vector<Ingredient> &getIngredients() const {
    return m_ingredients;
  }

  /**
   * @brief Get string representation of the pizza name.
   * @return The name of the pizza.
   */
  std::string getName() const;

  /**
   * @brief Get the string representation of the pizza size.
   * @return The size of the pizza.
   */
  std::string getSizeName() const;

protected:
  PizzaType m_type;
  PizzaSize m_size;
  std::vector<Ingredient> m_ingredients;
  int m_baseCookingTime;
};

/**
 * @class MargaritaPizza
 * @brief Class representing a Margarita pizza.
 */
class MargaritaPizza : public Pizza {
public:
  MargaritaPizza(PizzaSize size);
};

/**
 * @class ReginaPizza
 * @brief Class representing a Regina pizza.
 */
class ReginaPizza : public Pizza {
public:
  ReginaPizza(PizzaSize size);
};

/**
 * @class AmericanaPizza
 * @brief Class representing an Americana pizza.
 */
class AmericanaPizza : public Pizza {
public:
  AmericanaPizza(PizzaSize size);
};

/**
 * @class FantasiaPizza
 * @brief Class representing a Fantasia pizza.
 */
class FantasiaPizza : public Pizza {
public:
  FantasiaPizza(PizzaSize size);
};

/**
 * @brief Convert PizzaType to string.
 * @param type The type of the pizza.
 * @return The string representation of the pizza type.
 */
std::string toString(PizzaType type);

/**
 * @brief Convert PizzaSize to string.
 * @param size The size of the pizza.
 * @return The string representation of the pizza size.
 */
std::string toString(PizzaSize size);

/**
 * @brief Convert Ingredient to string.
 * @param ingredient The ingredient of the pizza.
 * @return The string representation of the ingredient.
 */
std::string toString(Ingredient ingredient);

/**
 * @brief Convert string to PizzaType.
 * @param type The string representation of the pizza type.
 * @return The corresponding PizzaType.
 * @throws ArgumentException if the string does not match any PizzaType.
 */
PizzaType pizzaTypeFromString(const std::string &type);

/**
 * @brief Convert string to PizzaSize.
 * @param size The string representation of the pizza size.
 * @return The corresponding PizzaSize.
 * @throws ArgumentException if the string does not match any PizzaSize.
 */
PizzaSize pizzaSizeFromString(const std::string &size);

} // namespace Plazza::Core
