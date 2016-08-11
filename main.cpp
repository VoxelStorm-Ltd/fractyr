#include <iostream>
#include <FTGL/ftgl.h>
#include "platform_defines.h"
#include "soundstorm/soundstorm.h"
#include "oculusstorm/oculusstorm.h"
#include "get_version.h"
#include "universe.h"
#include "gameplayer.h"

// globals
GLFWwindow *window_main = nullptr;                                              // the main game window
oculusstorm *oculus     = nullptr;                                              // oculus rift controller
FTFont *font_title      = nullptr;                                              // global font definitions
FTFont *font_loading    = nullptr;
// initialisation order important here:
soundstorm sound;                                                               // audio manager
gameplayer player;                                                              // player state and configuration
universe root;                                                                  // the container for everything physical

int main() {
  std::cout << "Fractyr version " << get_version() << " "
  #ifdef NDEBUG
    "Release"
  #else
    "Debug"
  #endif
  " GCC " __VERSION__ " for "
  #if defined PLATFORM_WINDOWS
    "Windows"
  #elif defined PLATFORM_MACOS
    "OS X"
  #elif defined PLATFORM_LINUX
    "Linux"
  #else
    "unknown OS"
  #endif
  #if defined PLATFORM_BIGENDIAN
    ", big endian"
  #elif defined PLATFORM_LITTLEENDIAN
    //", little endian"
  #else
    ", unspecified endianness"
  #endif
  << std::endl;

  try {
    root.init();
    root.loop_main();
    return EXIT_SUCCESS;
  } catch(std::exception const &e)  {
    std::cout << "Exception: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception!" << std::endl;
  }
  glfwTerminate();
  return EXIT_FAILURE;
}
