#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include "vectorstorm/vectorstorm.h"
#include "gameplayer.h"
#include "input/callbacks.h"

extern gameplayer player;

void callback_mousepos(GLFWwindow *thiswindow __attribute__((unused)), double x, double y) {
  /// React to mouse position updates
  player.move_mouse(vector2f(static_cast<float>(x), static_cast<float>(y)));
}

void callback_mousepos_pause(GLFWwindow *thiswindow __attribute__((unused)), double x __attribute__((unused)), double y __attribute__((unused))) {
  /// Pause mode
  // do nowt
}
