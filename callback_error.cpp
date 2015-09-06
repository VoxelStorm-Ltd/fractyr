#include <iostream>

void callback_error(int error, char const *description) {
  /// GLFW error reporting callback function
  try {
    std::cout << "ERROR: GLFW: " << error << ": " << description << std::endl;
  } catch(std::exception const &e) {
    std::cout << "ERROR: GLFW reported an error and additionally the parsing callback produced an exception: " << e.what() << ", GLFW error code " << error << std::endl;
  } catch(...) {
    std::cout << "ERROR: GLFW reported an error and additionally the parsing callback produced an unknown exception, GLFW error code " << error << std::endl;
  }
}
