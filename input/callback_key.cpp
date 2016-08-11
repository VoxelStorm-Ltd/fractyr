#include <iostream>
#include <GL/glew.h>
#include "gameplayer.h"
#include "universe.h"

extern GLFWwindow *window_main;
extern gameplayer player;
extern universe root;

void callback_key(GLFWwindow *thiswindow __attribute__((unused)),
                  int key,
                  int scancode __attribute__((unused)),
                  int action,
                  int mods __attribute__((unused))) {
  /// All the switch-type controls, rather than polled realtime controls
  if(action == GLFW_PRESS) {                                                    // here are all the down-presses we care about
    // these first key catches work in all input modes
    switch(key) {
    #ifndef NDEBUG
      case GLFW_KEY_O:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);                              // wireframe
        glDisable(GL_CULL_FACE);
        break;
      case GLFW_KEY_P:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);                              // filled
        glEnable(GL_CULL_FACE);
        break;
    #endif
    /*
    case GLFW_KEY_PRINT_SCREEN:   // prt scrn to take screenshots
      {
        screenshotstorm screenshot;
        screenshot.capture();
        std::string url = screenshot.upload();    // upload the existing file if saved
        if(url.size() == 0) {
          std::cout << "error: could not get upload URL!" << std::endl;
        } else {
          std::cout << "saved and uploaded to " << url << ", URL copied to clipboard" << std::endl;
          glfwSetClipboardString(window_main, url.c_str());
        }
      }
      break;
    */
    case GLFW_KEY_PAUSE:                                                        // pause the game
      root.state = universe::gamestate::PAUSED;
      std::cout << "Pause requested..." << std::endl;
      break;
    case GLFW_KEY_ESCAPE:                                                       // escape to quit
      root.state = universe::gamestate::QUITTING;
      std::cout << "Stop requested..." << std::endl;
      break;
    default:
      break;
    }
  }
}

void callback_key_pause(GLFWwindow *thiswindow __attribute__((unused)),
                        int key __attribute__((unused)),
                        int scancode __attribute__((unused)),
                        int action,
                        int mods __attribute__((unused))) {
  /// Controls specific to the paused state
  if(action == GLFW_PRESS) {                                                    // anything we press skips through the pause
    root.state = universe::gamestate::RUNNING;
  }
}
