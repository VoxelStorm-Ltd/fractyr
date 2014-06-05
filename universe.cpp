#include "universe.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>
#include "platform_defines.h"
#include "blob_loader.h"
#include "gameplayer.h"
#include "oculusstorm.h"
#include "soundstorm.h"
#include "world.h"
#include "buffer_chunk.h"
#include "buffer_plasma.h"
#include "buffer_enemy_grunt.h"
#include "ship.h"
#include "playership.h"
#include "blaster.h"
#include "plasma.h"
#include "grunt.h"
#include "chunk.h"

// loaders
FTFont *font_load(  std::string const &filename, unsigned int size = 16);
FTFont *font_load3d(std::string const &filename, unsigned int size = 16);
FTFont *font_load(  unsigned char const *buffer, size_t buffersize, unsigned int size = 16);
FTFont *font_load3d(unsigned char const *buffer, size_t buffersize, unsigned int size = 16);

// callbacks
void callback_mousepos(             GLFWwindow *thiswindow, double x, double y);
void callback_mousepos_pause(       GLFWwindow *thiswindow, double x, double y);
void callback_mousebutton(          GLFWwindow *thiswindow, int button, int action, int mods);
void callback_key(                  GLFWwindow *thiswindow, int key, int scancode, int action, int mods);
void callback_key_pause(            GLFWwindow *thiswindow, int key, int scancode, int action, int mods);
void callback_scroll(               GLFWwindow *thiswindow, double xoffset, double yoffset);
void callback_windowresize(         GLFWwindow *thiswindow, int newwidth, int newheight);
void callback_windowrefresh(        GLFWwindow *thiswindow);
void callback_windowrefresh_minimal(GLFWwindow *thiswindow);
void callback_windowclose(          GLFWwindow *thiswindow);
void callback_windowfocus(          GLFWwindow *thiswindow, int focused);
void callback_windowminimise(       GLFWwindow *thiswindow, int minimised);

// globals
extern gameplayer player;
extern soundstorm sound;
extern GLFWwindow *window_main;
extern oculusstorm *oculus;
//extern FTFont *font_label;

std::mt19937 universe::randomgen;
unsigned int universe::randomseed = 1;

universe::universe()
  : time_fpsupdate(std::chrono::high_resolution_clock::now()),
    timestep_chrono(std::chrono::milliseconds(static_cast<unsigned int>(timestep * 1000) - 1)) { // -1 to go a bit over
  /// Default constructor
  std::cout << "Initialising universe..." << std::endl;
  //sound.set_listener_position_and_rotation(Vector3f(0.0, 0.0, 0.0), Quatf::fromEulerAngles(0.0, 180.0, 0.0));   // so that -x = left, +x = right
  // initialise sound effects
  // TODO
  // initialise music
  // TODO
  sound.start_streamer();                 // only do this after all music has been loaded
}

universe::~universe() {
  /// Default destructor
  // no reason to waste user time at exit by cleaning up things that don't affect hardware
  delete_shaders();                             // release shader programs
  delete_buffers();                             // release graphics buffers
}

void universe::init() {
  /// Runtime initialisation that must be run before main loops, but cannot occur in the global constructor for whatever reason
  Vector2i windowsize(800, 600);
  init_graphics(windowsize);
  init_shaders();

  player.update_window(windowsize);
  player.setup_input();

  set_graphicslevel(graphicsleveltype::NICEST);
  init_buffers();                               // needs to come before progress screen
  render_progressscreen(0.0, "Loading...");

  restart();

  std::cout << "Initialisation complete." << std::endl;
  // this must be absolutely last:
  glfwSetTime(0.0);           // reset the timer for the start of the main loop
}

void universe::restart() {
  /// Set up the universe to an initial state
  randomgen.seed(randomseed);
  srand(randomseed);

  delete current_world;
  current_world = new world();

  // world content setup
  player.current_ship = new playership(*current_world, current_world->get_chunk(Vector3i(world::size/2, world::size/2, world::size/2)), Vector3f(70.0, 10.0, 10.0));
  player.current_ship->add_weapon(new blaster(player.current_ship, 10.0, 1.5));

  glfwSetInputMode(            window_main, GLFW_CURSOR, GLFW_CURSOR_NORMAL);     // release the cursor
  glfwSetCursorPosCallback(    window_main, callback_mousepos);
  glfwSetMouseButtonCallback(  window_main, callback_mousebutton);
  glfwSetKeyCallback(          window_main, callback_key);
  glfwSetScrollCallback(       window_main, callback_scroll);
  glfwSetWindowSizeCallback(   window_main, callback_windowresize);
  glfwSetWindowRefreshCallback(window_main, callback_windowrefresh_minimal);
  glfwSetWindowCloseCallback(  window_main, callback_windowclose);
  glfwSetWindowFocusCallback(  window_main, callback_windowfocus);
  glfwSetWindowIconifyCallback(window_main, callback_windowminimise);
  glfwSetInputMode(window_main, GLFW_CURSOR, GLFW_CURSOR_DISABLED);     // for mouselook

  sound.music_clear(0);                         // clear the decks
  sound.music_clear(1);
  //sound.music_queue(0, 0);                      // start the music
  //sound.music_queue(0, 1);
  //sound.music_queue(1, 2);
  //sound.music_queue(1, 3);
  sound.set_music_volume(0, 1.0);
  sound.set_music_volume(1, 0.0);
}

void universe::init_buffers() {
  /// Allocate new buffers after context switch
  if(current_world) {
    current_world->setup_buffers();
  }
  plasma::buf.init();
  plasma::buf.setup();
  grunt::buf.init();
  grunt::buf.setup();

  // initialise fonts
  // TODO
}

void universe::init_shaders() {
  /// Load and initialise shader programs
  plasma::buf.load_shader();
  grunt::buf.load_shader();
}

void universe::delete_buffers() {
  /// Unallocate all existing buffers in preparation for context switch
  /// Note: this should be safe to call even on null buffers
  if(current_world) {
    current_world->delete_buffers();
  }
  plasma::buf.destroy();
  grunt::buf.destroy();
}

void universe::delete_shaders() {
  /// Clean up all loaded shaders
  buffer_chunk::destroy_shader();
  plasma::buf.destroy_shader();
  grunt::buf.destroy_shader();
}

void universe::render() {
  /// Draw everything
  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // TODO: don't bother clearing colour buffer

  if(__builtin_expect(!player.current_ship, 0)) {     // branch prediction: unlikely that the player will have no ship
    return;
  }
  player.current_ship->render_from();
  render_energy_hud(player.current_ship->energy / playership::max_energy);
}

void universe::render_progressscreen(float progress, std::string const &message) {
  /// Render a single frame of a progress bar, values from 0 to 1
  glfwPollEvents();             // poll events so the OS doesn't think we're unresponsive.
  if(oculus->enabled) {
    // render once for each eye
    player.setup_render_oculus_left_locked();
    render();
    render_progressscreen_hud(progress, message);
    player.setup_render_oculus_right_locked();
    render();
    render_progressscreen_hud(progress, message);
  } else {
    player.setup_render_perspective();
    render();
    render_progressscreen_hud(progress, message);
  }
  glfwSwapBuffers(window_main);
}

void universe::render_progressscreen_hud(float progress, std::string const &message) {
  Vector2i const windowsize(player.get_windowsize());

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, windowsize.x, 0, windowsize.y, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);

  //float const advance = font_label->Advance(message.c_str(), message.length());
  //Vector2i messagepos((windowsize.x - advance) / 2.0, (windowsize.y * 0.25) + 10.0);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  //font_label->Render(message.c_str(), message.length(), FTPoint(messagepos.x, messagepos.y), FTPoint(), FTGL::RENDER_FRONT);
  glColor4f(0.0, 1.0, 0.0, 1.0);
  glBegin(GL_LINES);
  glVertex2i(windowsize.x * 0.25,                      (windowsize.y * 0.25) - 10.0);
  glVertex2i(windowsize.x * (0.25 + (progress * 0.5)), (windowsize.y * 0.25) - 10.0);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
  glEnable(GL_DEPTH_TEST);
}

void universe::render_energy_hud(float energy) {
  Vector2i const windowsize(player.get_windowsize());

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, windowsize.x, 0, windowsize.y, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);

  glColor4f(0.0, 0.0, 1.0, 0.8);
  glLineWidth(10.0);
  glBegin(GL_LINES);
  glVertex2i(windowsize.x * 0.25,                    5.0);
  glVertex2i(windowsize.x * (0.25 + (energy * 0.5)), 5.0);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
  glEnable(GL_DEPTH_TEST);
}

void universe::set_graphicslevel(graphicsleveltype newlevel) {
  /// Adjust the graphics settings
  if(newlevel != graphicslevel) {         // reinitialise the window, if necessary
    std::cout << "Level changed, old " << (int)graphicslevel << " new " << (int)newlevel << std::endl;
    switch(newlevel) {                    // set the broad options here
    case graphicsleveltype::NICEST:
      antialiasing = 8;
      glEnable(GL_MULTISAMPLE);
      break;
    case graphicsleveltype::COMPROMISE:
      antialiasing = 2;
      glEnable(GL_MULTISAMPLE);
      break;
    case graphicsleveltype::FASTEST:
      antialiasing = 0;
      glDisable(GL_MULTISAMPLE);
      break;
    }
    graphicslevel = newlevel;
    reinitialise_window();
  } else {
    //std::cout << "Level unchanged, old " << (int)graphicslevel << " new " << (int)newlevel << std::endl;
    graphicslevel = newlevel;
  }
  switch(newlevel) {                      // always carry these out again
  case graphicsleveltype::NICEST:
    glShadeModel(GL_SMOOTH);  // SMOOTH or FLAT
    glHint(GL_GENERATE_MIPMAP_HINT,       GL_NICEST);       // GL_FASTEST, GL_NICEST or GL_DONT_CARE
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    break;
  case graphicsleveltype::COMPROMISE:
    glShadeModel(GL_SMOOTH);  // SMOOTH or FLAT
    glHint(GL_GENERATE_MIPMAP_HINT,       GL_DONT_CARE);    // GL_FASTEST, GL_NICEST or GL_DONT_CARE
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_DONT_CARE);
    break;
  case graphicsleveltype::FASTEST:
    glShadeModel(GL_SMOOTH);  // SMOOTH or FLAT
    glHint(GL_GENERATE_MIPMAP_HINT,       GL_FASTEST);      // GL_FASTEST, GL_NICEST or GL_DONT_CARE
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_FASTEST);
    break;
  }
}

void universe::toggle_fullscreen() {
  /// Switch between windowed and fullscreen modes
  if(oculus->enabled) {
    std::cout << "Ignoring full-screen switch request while in Oculus Rift mode." << std::endl;
    return;
  }
  fullscreen = !fullscreen;
  if(fullscreen) {
    std::cout << "Switched to full-screen mode." << std::endl;
  } else {
    std::cout << "Switched to windowed mode." << std::endl;
  }
  reinitialise_window();
}

void universe::reinitialise_window() {
  /// Force the window to be recreated - necessary for graphcis context changes
  std::cout << "Freeing compiled shader programs..." << std::endl;
  delete_shaders();
  std::cout << "Freeing allocated graphics buffers..." << std::endl;
  delete_buffers();
  std::cout << "Terminating GLFW..." << std::endl;
  glfwDestroyWindow(window_main);
  window_main = nullptr;
  glfwTerminate();
  std::cout << "Re-initialising GLFW and graphics..." << std::endl;
  Vector2i windowsize(800, 600);
  init_graphics(windowsize);
  init_shaders();
  player.update_window(windowsize);
  set_graphicslevel(graphicslevel);
  std::cout << "Re-initialising graphics buffers..." << std::endl;
  init_buffers();

  glfwSetKeyCallback(window_main, callback_key);
  glfwSetWindowRefreshCallback(window_main, callback_windowrefresh);
  glfwSetInputMode(window_main, GLFW_CURSOR, GLFW_CURSOR_DISABLED);     // for mouselook
}

void universe::loop_menu() {
  /// The between-game menu
}

void universe::loop_main() {
  /// the main rendering loop
  //loop_menu();
  while(state == gamestate::RUNNING || state == gamestate::PAUSED) {
    glfwSetKeyCallback(window_main, callback_key);
    glfwSetCursorPosCallback(window_main, callback_mousepos);
    glfwSetWindowRefreshCallback(window_main, callback_windowrefresh);

    while(state == gamestate::RUNNING) {
      glfwPollEvents();

      update();

      if (player.current_ship) {
        if (player.current_ship && player.current_ship->energy <= 0) {
          state = gamestate::LOST;
        }
      }

      if(oculus->enabled) {
        // render once for each eye
        player.setup_render_oculus_left();
        render();
        player.render_hud();
        player.setup_render_oculus_right();
        render();
        player.render_hud();
      } else {
        player.setup_render_perspective();
        render();
        player.render_hud();
      }
      glfwSwapBuffers(window_main);

      #ifndef NDEBUG
        // fps counter update
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> const time_now(std::chrono::high_resolution_clock::now());
        // measure performance margin
        std::chrono::duration<double> const timeleft(timenexttickstart - time_now);
        double const timeleft_ratio = timeleft / timestep_chrono;
        if(time_now >= time_fpsupdate) {
          fps = frames_last_interval / fpsinterval;
          frames_last_interval = 0;
          time_fpsupdate = time_now + std::chrono::duration<double>(std::chrono::milliseconds(static_cast<int>(1000 * fpsinterval)));
          //std::cout << "FPS: " << fps << std::endl;
          if(timeleft_ratio < 0.25) {
            if(timeleft_ratio < 0) {
              std::cout << "Perfmon: < frame limiter by " << -timeleft_ratio * 100 << "% (" << std::chrono::duration_cast<std::chrono::milliseconds>(timeleft).count() / 1000.0 << "ms)!  FPS " << fps << std::endl;
            } else {
              std::cout << "Perfmon: margin < " << timeleft_ratio * 100 << "% (" << std::chrono::duration_cast<std::chrono::milliseconds>(timeleft).count() / 1000.0 << "ms) per frame" << std::endl;
            }
          } else {
            //std::cout << "Perfmon: margin " << timeleft_ratio * 100 << "% (" << std::chrono::duration_cast<std::chrono::milliseconds>(timeleft).count() / 1000.0 << "ms) remaining per frame" << std::endl;
          }
        } else {
          ++frames_last_interval;
        }
      #endif

      // fps cap
      std::this_thread::sleep_until(timenexttickstart);
      timenexttickstart = std::chrono::high_resolution_clock::now() + timestep_chrono;
    }

    // find out why we exited the loop
    switch(state) {
    case gamestate::PAUSED:
      std::cout << "Paused" << std::endl;
      loop_pause();
      std::cout << "Resumed" << std::endl;
      break;
    case gamestate::QUITTING:
      std::cout << "Exiting at player's request." << std::endl;
      loop_menu();
      break;
    case gamestate::WON:
      std::cout << "Win." << std::endl;
      loop_menu();
      break;
    case gamestate::LOST:
      std::cout << "Lost." << std::endl;
      loop_menu();
      break;
    case gamestate::MENU:
      std::cout << "Finished loop from menu" << std::endl;
      state = gamestate::RUNNING;
      break;
    default:
      std::cout << "ERROR: unknown game state " << static_cast<int>(state) << std::endl;
      state = gamestate::QUITTING;
      break;
    }
  }
  std::cout << "Have a nice day!" << std::endl;
}

void universe::loop_pause() {
  /// Show a responsive pause screen, waiting for the player's command to resume
  glfwSetInputMode(window_main, GLFW_CURSOR, GLFW_CURSOR_NORMAL);     // release the cursor
  glfwSetKeyCallback(window_main, callback_key_pause);
  glfwSetCursorPosCallback(window_main, callback_mousepos_pause);
  while(state == gamestate::PAUSED) {
    glfwPollEvents();
    if(oculus->enabled) {
      // render once for each eye
      player.setup_render_oculus_left();
      render();
      player.setup_render_oculus_right();
      render();
    } else {
      player.setup_render_perspective();
      render();
    }
    glfwSwapBuffers(window_main);

    // fps cap
    std::this_thread::sleep_until(timenexttickstart);
    timenexttickstart = std::chrono::high_resolution_clock::now() + timestep_chrono;
  }
  if(state != gamestate::QUITTING) {
    state = gamestate::RUNNING;     // reset to running mode
  }
  glfwSetInputMode(window_main, GLFW_CURSOR, GLFW_CURSOR_DISABLED);     // for mouselook
}

void universe::update() {
  /// Update the physics of everything in this universe
  if(__builtin_expect(!!(current_world), 0)) {     // branch prediction: unlikely that there will be no current world
    // cast needed because not negating the pointer breaks branch prediction, see http://ideone.com/T6oy78
    current_world->update();
  }
  // update player position last - this way control inputs are as recent as possible for the next frame
  player.update();
}

chunk *universe::get_chunk(Vector3i const &chunk_coords) {
  /// Wrapper function
  #ifndef NDEBUG
    if(!current_world) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << " called on null current_world!  Clean this up for release." << std::endl;
      return nullptr;
    }
  #endif
  return current_world->get_chunk(chunk_coords);
}
