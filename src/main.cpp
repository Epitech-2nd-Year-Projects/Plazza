#include "Exceptions/ArgumentException.hpp"
#include "Reception/Reception.hpp"
#include <chrono>
#include <iostream>

static void printUsage(char **argv) {
  std::cerr << "Usage: " << argv[0]
            << " <time_multiplier> <cooks_per_kitchen> <stock_regen_time_ms>"
            << std::endl;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    printUsage(argv);
    return 84;
  }

  try {
    double timeMultiplier = std::stod(argv[1]);
    uint32_t cooksPerKitchen = std::stoul(argv[2]);
    uint32_t stockRestockTime = std::stoul(argv[3]);

    if (timeMultiplier <= 0) {
      throw Plazza::Exceptions::ArgumentException(
          "Time multiplier must be a positive number");
    }
    if (cooksPerKitchen == 0) {
      throw Plazza::Exceptions::ArgumentException(
          "Number of cooks must be a positive number");
    }

    auto stockRegenTime = std::chrono::milliseconds(stockRestockTime);

    Plazza::Reception::Reception reception(timeMultiplier, cooksPerKitchen,
                                           stockRegenTime);
    reception.run();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 84;
  }
  return 0;
}
