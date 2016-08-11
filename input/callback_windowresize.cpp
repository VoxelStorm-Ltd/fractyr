#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include "vectorstorm/vectorstorm.h"
#include "gameplayer.h"
#include "universe.h"

extern gameplayer player;
extern universe root;

void callback_windowresize(GLFWwindow *thiswindow, int newwidth, int newheight) {
  /// Callback for handling window resize events
  player.update_window(vector2i(newwidth, newheight));
  // update the window while dragging, no need to take rift render mode into account as it's full-screen only
  player.setup_render_perspective();
  root.render();
  if(root.state == universe::gamestate::RUNNING) {
    player.render_hud();
  }
  glfwSwapBuffers(thiswindow);
}
