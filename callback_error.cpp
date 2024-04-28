#include "callback_error.h"
#include <iostream>

void callback_error(int error, char const *description) {
  /// GLFW error reporting callback function
  std::cerr << "ERROR: GLFW: " << error << ": " << description << std::endl;
}
