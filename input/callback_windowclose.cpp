#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "universe.h"

extern universe root;

void callback_windowclose(GLFWwindow *thiswindow __attribute__((unused))) {
  /// Callback for handling window close events
  std::cout << "Window closed, exiting" << std::endl;
  root.state = universe::gamestate::QUITTING;
}
