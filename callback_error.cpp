#include <iostream>

void callback_error(int error, char const *description) {
  /// GLFW error reporting callback function
  std::cout << "ERROR: GLFW: " << error << ": " << description << std::endl;
}
