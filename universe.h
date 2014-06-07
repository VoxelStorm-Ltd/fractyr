#ifndef UNIVERSE_H_INCLUDED
#define UNIVERSE_H_INCLUDED

#include <chrono>
#include <thread>
#include <list>
#include <vector>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vmath.h"

class world;
class chunk;

class universe {
private:
  // fps counter setup
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> time_fpsupdate;
  static double constexpr fpsinterval = 10.0;           // how many seconds between reporting performance
  unsigned int frames_last_interval = 0;
  static double constexpr fpscap = 60.0;
  static double constexpr timestep = 1.0 / fpscap;      // assume we're running at a fixed 60fps
  std::chrono::duration<double> const timestep_chrono;
  std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> timenexttickstart;

public:
  enum class graphicsleveltype : char {
    NICEST,
    COMPROMISE,
    FASTEST
  };
  /// FSAA modes (see http://stackoverflow.com/questions/6129797/how-to-get-gpu-multisampling-modes-descriptions)
  ///    0 - Off GL_SAMPLE_BUFFERS = 0  GL_SAMPLES = 0
  ///    1 - 2x  (2xMS)                 GL_SAMPLES = 1
  ///    2 - ?                          GL_SAMPLES = 2
  ///    3 - ?                          GL_SAMPLES = 2
  ///    4 - ?                          GL_SAMPLES = 4
  ///    5 - 4x  (4xMS)                 GL_SAMPLES = 4
  ///    6 - ?                          GL_SAMPLES = 6
  ///    7 - 8x  (4xMS, 4xCS)           GL_SAMPLES = 6
  ///    8 - 16x (4xMS, 12xCS)          GL_SAMPLES = 6
  ///    9 - 8x  (4xSS, 2xMS)           GL_SAMPLES = 6
  ///   10 - 8x  (8xMS)                 GL_SAMPLES = 6
  ///   12 - 16x (8xMS, 8xCS)           GL_SAMPLES = 6
  GLint antialiasing = 1;

  graphicsleveltype graphicslevel = graphicsleveltype::NICEST;

  enum class gamestate : char {
    RUNNING,
    PAUSED,
    QUITTING,
    WON,
    LOST,
    MENU
  };
  gamestate state = gamestate::RUNNING;                 // what to do with the main loop
  unsigned int fps = 0;                                 // fps counter
  #ifndef NDEBUG
    bool fullscreen = false;                            // whether we're currently running fullscreen or windowed
  #else
    bool fullscreen = true;                             // fullscreen default for release build
  #endif

  static std::mt19937 randomgen;                        // global random number generator
  static unsigned int randomseed;                       // global random seed for this session

  world *current_world = nullptr;                       // the current level

  static unsigned int constexpr numgrunts = 300;
  static unsigned int constexpr numcores = 20;
  static unsigned int constexpr cores_to_win = 5;   // number of cores required to win

  static unsigned int constexpr fadetime    = 120;
  static unsigned int constexpr fadetimewon = 300;

  universe();
  ~universe();

  void init();
private:
  void init_graphics(Vector2i &windowsize);
public:
  void restart();
  void replace_entities();

private:
  // main program loops
  void loop_menu();
  void loop_fade_in();
  void loop_fade_out();
  void loop_fade_out_won();
public:
  void loop_main();
private:
  void loop_pause();

  // render
  void init_buffers();
  void init_shaders();
  void delete_buffers();
  void delete_shaders();
public:
  void render();
  void render_progressscreen(    float progress, std::string const &message = std::string("Loading..."));
  void render_progressscreen_hud(float progress, std::string const &message = std::string("Loading..."));
  void render_energy_hud(float energy);
  void render_cores_hud(unsigned int cores);
  void set_graphicslevel(graphicsleveltype newlevel);
  void toggle_fullscreen();
  void reinitialise_window();

  // game cycle & physics
  void update();

  // chunk operations and queries
  chunk *get_chunk(Vector3i const &chunk_coords);
};

#endif // UNIVERSE_H_INCLUDED
