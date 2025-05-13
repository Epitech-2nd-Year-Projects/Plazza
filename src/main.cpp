#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "Argc: " << argc << std::endl;
  for (int i = 0; i < argc; ++i) {
    std::cout << "Argv[" << i << "]: " << argv[i] << std::endl;
  }
  return 0;
}
