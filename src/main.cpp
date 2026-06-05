#include "core/Application.h"
#include <iostream>

int main() {
  try {
    Application app("ArcStudio 0.1.0", 1200, 800);
    app.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return -1;
  }
  return 0;
}