//#include <iostream>
//#include <cmath>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "gameplayer.h"
#include "input/callbacks.h"

extern gameplayer player;

void callback_scroll(GLFWwindow *thiswindow __attribute__((unused)), double xoffset, double yoffset) {
  /// Apply relevant action for using the mouse scroll
  for(auto &binding : player.bindings) {
    if(binding.type != gameplayer::controlbinding::controltype::MOUSE_SCROLL) {
      continue;
    }
    if(yoffset != 0.0) {
      // scroll up and down
      #ifdef DEBUG_INPUTSTORM
        std::cout << "DEBUG: inputstorm: scrolled vertically " << yoffset << std::endl;
      #endif
      if(binding.control == 0) {
        (player.*binding.function)(yoffset * binding.scale);
      }
    }
    if(xoffset != 0.0) {
      // left and right scroll - this doesn't always work for all mice and trackpads
      #ifdef DEBUG_INPUTSTORM
        std::cout << "DEBUG: inputstorm: scrolled sideways " << xoffset << std::endl;
      #endif
      if(binding.control == 1) {
        (player.*binding.function)(xoffset * binding.scale);
      }
    }
  }
}
