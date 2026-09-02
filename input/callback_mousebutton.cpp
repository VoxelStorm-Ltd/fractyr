//#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "universe.h"
#include "gameplayer.h"
#include "input/callbacks.h"

extern universe root;
extern gameplayer player;

void callback_mousebutton(GLFWwindow *thiswindow __attribute__((unused)),
                          int button,
                          int action,
                          int mods __attribute__((unused))) {
  /// React to mouse button presses and releases
  switch(button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    if(action == GLFW_PRESS) {                                                  // mouse 1 down
      // TODO
    }
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    if(action == GLFW_PRESS) {                                                  // mouse 2 down
      // TODO
    }
    break;
  case GLFW_MOUSE_BUTTON_MIDDLE:
    break;
  default:
    if(action == GLFW_PRESS) {
      //std::cout << "DEBUG: Detected mouse button press on unbound button id " << button << std::endl;
    }
    break;
  }
}
