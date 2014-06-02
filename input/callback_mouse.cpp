#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vmath.h"
#include "gameplayer.h"

extern gameplayer player;

void callback_mousepos(GLFWwindow *thiswindow __attribute__((unused)), double x, double y) {
  /// React to mouse position updates
  player.move_mouse(Vector2f(x, y));
}

void callback_mousepos_pause(GLFWwindow *thiswindow __attribute__((unused)), double x __attribute__((unused)), double y __attribute__((unused))) {
  /// Pause mode
  // do nowt
}
