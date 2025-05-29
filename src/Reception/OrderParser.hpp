/**
 * @file OrderParser.hpp
 * @brief Defines the OrderParser class for parsing pizza orders.
 */

#pragma once

#include "Communication/Serialization.hpp"
#include <regex>
#include <string>
#include <vector>

namespace Plazza::Reception {
/**
 * @class OrderParser
 * @brief Parses pizza orders from a string input.
 */
class OrderParser {
public:
  /**
   * @brief Parses a pizza order from a string input.
   * @param input The input string containing the order.
   * @return A vector of PizzaOrder objects representing the parsed orders.
   * @throws Exceptions::ParserException if the input format is invalid.
   * @throws Exceptions::ArgumentException if the pizza type or size is invalid.
   */
  static std::vector<Communication::PizzaOrder>
  parseOrder(const std::string &input);

  /**
   * @brief Validates the format of a pizza order string.
   * @param input The input string to validate.
   * @return True if the input is a valid order format, false otherwise.
   */
  static bool isValidOrder(const std::string &input);

private:
  static const std::regex ORDER_REGEX;
  static uint32_t m_nextOrderId;
};
} // namespace Plazza::Reception
