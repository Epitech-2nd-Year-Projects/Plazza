#include "Reception/OrderParser.hpp"
#include "Core/Pizza.hpp"
#include "Exceptions/ParserException.hpp"
#include <sstream>

namespace Plazza::Reception {
const std::regex
    OrderParser::ORDER_REGEX(R"(([a-zA-Z]+)\s+(S|M|L|XL|XXL)\s+x(\d+))",
                             std::regex_constants::icase);

uint32_t OrderParser::m_nextOrderId = 1;

std::vector<Communication::PizzaOrder>
OrderParser::parseOrder(const std::string &input) {
  std::vector<Communication::PizzaOrder> orders;

  std::istringstream stream(input);
  std::string orderPart;

  while (std::getline(stream, orderPart, ';')) {
    orderPart.erase(0, orderPart.find_first_not_of(" \t"));
    orderPart.erase(orderPart.find_last_not_of(" \t") + 1);

    if (orderPart.empty())
      continue;

    std::smatch match;
    if (std::regex_match(orderPart, match, ORDER_REGEX)) {
      try {
        std::string orderTypeString = match[1].str();
        std::string orderSizeString = match[2].str();
        uint32_t orderQuantity = std::stoul(match[3].str());

        Core::PizzaType type = Core::pizzaTypeFromString(orderTypeString);
        Core::PizzaSize size = Core::pizzaSizeFromString(orderSizeString);

        for (uint32_t i = 0; i < orderQuantity; ++i) {
          Communication::PizzaOrder order;
          order.type = type;
          order.size = size;
          order.quantity = 1;
          order.orderId = m_nextOrderId++;
          orders.push_back(order);
        }

      } catch (const std::exception &e) {
        throw Exceptions::ParserException("Failed to parse order part '" +
                                          orderPart + "': " + e.what());
      }
    } else {
      throw Exceptions::ParserException("Invalid order format: '" + orderPart +
                                        "'. Expected format: "
                                        "<PizzaType> <Size> x<Quantity>");
    }
  }

  if (orders.empty()) {
    throw Exceptions::ParserException(
        "No valid pizza orders found in input: '" + input + "'");
  }

  return orders;
}

bool OrderParser::isValidOrder(const std::string &input) {
  try {
    parseOrder(input);
    return true;
  } catch (...) {
    return false;
  }
}
} // namespace Plazza::Reception
