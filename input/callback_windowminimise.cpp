#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "universe.h"
#include "gameplayer.h"

extern universe root;
extern gameplayer player;

void callback_mousepos(      GLFWwindow *thiswindow, double x, double y);
void callback_mousepos_pause(GLFWwindow *thiswindow, double x, double y);

void callback_windowminimise(GLFWwindow *thiswindow __attribute__((__unused__)), int minimised) {
  /// Callback for handling window minimise events
  if(minimised == GL_TRUE) {
    root.state = universe::gamestate::PAUSED;
    //player.mouse_last = player.cursorpos;
    glfwSetCursorPos(thiswindow, player.cursorpos.x, player.get_windowsize().y - player.cursorpos.y);
    glfwSetCursorPosCallback(thiswindow, callback_mousepos_pause);
  } else {
    if(root.state != universe::gamestate::PAUSED) {
      return;                                                                   // don't change states if we aren't paused
    }
    root.state = universe::gamestate::RUNNING;
    //player.mouse_last = player.cursorpos;
    glfwSetCursorPos(thiswindow, player.cursorpos.x, player.get_windowsize().y - player.cursorpos.y);
    glfwSetCursorPosCallback(thiswindow, callback_mousepos);
  }
}
