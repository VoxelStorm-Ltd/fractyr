#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include "vectorstorm/vectorstorm.h"
#include "gameplayer.h"
#include "universe.h"
#include "oculusstorm/oculusstorm.h"

extern gameplayer player;
extern universe root;
extern oculusstorm *oculus;

void callback_windowrefresh(GLFWwindow *thiswindow) {
  /// Callback to handle refresh requests
  if(oculus->enabled) {
    // render once for each eye
    player.setup_render_oculus_left();
    root.render();
    player.setup_render_oculus_right();
    root.render();
  } else {
    player.setup_render_perspective();
    root.render();
    player.render_hud();
  }
  glfwSwapBuffers(thiswindow);
}

void callback_windowrefresh_minimal(GLFWwindow *thiswindow) {
  /// Alternative refresh without perspective adjustment or hud
  glfwSwapBuffers(thiswindow);
}
