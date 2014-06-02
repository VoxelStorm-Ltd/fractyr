#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "universe.h"
#include "gameplayer.h"

extern universe root;
extern gameplayer player;

void callback_mousepos(      GLFWwindow *thiswindow, double x, double y);
void callback_mousepos_pause(GLFWwindow *thiswindow, double x, double y);

void callback_windowfocus(GLFWwindow *thiswindow __attribute__((__unused__)), int focused) {
  /// Callback for handling window focus and unfocus
  if(focused == GL_TRUE) {
    if(root.state != universe::gamestate::PAUSED) {
      return;       // don't change states if we aren't paused
    }
    root.state = universe::gamestate::RUNNING;
    //player.mouse_last = player.cursorpos;
    glfwSetCursorPos(thiswindow, player.cursorpos.x, player.get_windowsize().y - player.cursorpos.y);
    glfwSetCursorPosCallback(thiswindow, callback_mousepos);
  } else {
    root.state = universe::gamestate::PAUSED;
    //player.mouse_last = player.cursorpos;
    glfwSetCursorPos(thiswindow, player.cursorpos.x, player.get_windowsize().y - player.cursorpos.y);
    glfwSetCursorPosCallback(thiswindow, callback_mousepos_pause);
  }
}
