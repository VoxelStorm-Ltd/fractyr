#ifndef GAMEPLAYER_H_INCLUDED
#define GAMEPLAYER_H_INCLUDED

#include <vector>
#include <deque>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vmath.h"

class playership;                 // forward dec

class gameplayer {
private:
  Vector2f mouse_sensitivity;     // degrees per pixel moved for both horizontal and vertical mouse movement
  Vector2f mouse_last;            // mouse coords relative to origin
  Vector2i windowsize;            // in pixels, set by glfw - modifying this directly changes nothing
  static float constexpr fov_limit_max = 50.0;        // maximum field of view
  static float constexpr fov_limit_min = 10.0;        // maximum zoom
  float fov_angle = 0.0;          // cached field of view, degrees
  float fov_angle_rad = 0.0;      // cached field of view, rads
  float fov_ratio = 1.0;          // updated by update_fov_ratio() from degrees
  float aspect_ratio = 1.0;       // updated by update_aspect_ratio() from windowsize
  float pre_rotation = 0.0;       // fixed rotation applied before head rotation or mouselook, degrees
  float rotation_yaw = 0.0;       // euler angles for head rotation - we want gimbal limitation here
  float rotation_pitch = 0.0;     //   these are in degrees per pixel

  bool show_help = false;
  bool show_hud  = true;

  std::vector<std::string> helptext;                   // help screen text

  Matrix4f projection;            // cached projection matrix

public:
  Vector2f cursorpos;             // on-screen cursor position

  // state
  playership *current_ship = nullptr; // what entity we're currently controlling

  // input
  struct controlbinding {
    enum class controltype : char {
      KEYBOARD,                   // keyboard key
      MOUSE_AXIS,                 // x or y axis on the mouse
      MOUSE_BUTTON,               // a mouse button
      MOUSE_SCROLL,               // x or y mouse scroll axis
      JOYSTICK_AXIS_FULL,         // a joystick axis
      JOYSTICK_AXIS_HALF_POS,     // the positive half of a joystick axis only
      JOYSTICK_AXIS_HALF_NEG,     // the negative half of a joystick axis only
      JOYSTICK_BUTTON             // a joystick button
    };
    typedef void (gameplayer::*inputfunctiontype)(float);
    /// A binding between input controls and a function
    controltype type;             // what type of binding this is
    int control;                  // the key or axis id
    inputfunctiontype function;   // the function to call
    float deadzone;               // axis deadzone, if applicable
    float scale;                  // axis scale multiplier (also use this for invert)
    bool repeat;                  // whether holding the key keeps activating, or only calls once
    bool helddown;                // used for repeat control
  };
  std::vector<controlbinding> bindings;         // all of the input bindings
  std::vector<int>gamepads;       // IDs of all gamepads available
  int gamepad = -1;               // gamepad ID for this player, -1 for none attached.
  int axiscount = 0;
  int buttoncount = 0;
  float const *axes = nullptr;
  unsigned char const *buttons = nullptr;

  #ifndef NDEBUG
    bool invisible = false;
    bool noclip    = false;
  #endif

  gameplayer();
  ~gameplayer();

  // render setup
private:
  void cache_matrix(double nearplane = 1.0, double farplane = 1000.0);
public:
  void setup_render_perspective();
  void setup_render_oculus_left();
  void setup_render_oculus_right();
  void setup_render_oculus_left_locked();
  void setup_render_oculus_right_locked();
  void setup_render_ortho();
  void setup_render_ortho_restore();

  // render
  void render_hud();

  // view adjustments
  void update_fov(double fov = 90);
  void update_fov_ratio();
  Vector2i const &get_windowsize() const;
  void update_window(Vector2i const &newwindowsize);
  void update_aspect_ratio();
  void clamp_pitch();
  Vector3f const &get_position() const;
  void set_position(Vector3f const &newposition);
  float const &get_rotation_yaw() const;
  float const &get_rotation_pitch() const;

  // world interaction and input
  void setup_input();
  void select_gamepad(int newgamepad);
  void pollcontrols(GLFWwindow *thiswindow);
  void move_mouse(Vector2f const &mouse_pos);
  void bind(controlbinding::controltype type, int control, controlbinding::inputfunctiontype function, bool repeat = true, float deadzone = 0.0, float scale = 1.0);
  void unbind(controlbinding::controltype type, int control);
  void unbind(controlbinding::inputfunctiontype function);
  void update();

  // input-driven actions
  void input_move_forward(       float amount = 1.0);
  void input_move_back(          float amount = 1.0);
  void input_move_left(          float amount = 1.0);
  void input_move_right(         float amount = 1.0);
  void input_move_up(            float amount = 1.0);
  void input_move_down(          float amount = 1.0);
  void input_roll_left(          float amount = 1.0);
  void input_roll_right(         float amount = 1.0);
  void input_yaw_left(           float amount = 1.0);
  void input_yaw_right(          float amount = 1.0);
  void input_pitch_up(           float amount = 1.0);
  void input_pitch_down(         float amount = 1.0);
  void input_view_left(          float amount = 1.0);
  void input_view_right(         float amount = 1.0);
  void input_zoom_in(            float amount = 1.0);
  void input_zoom_out(           float amount = 1.0);
  void input_zoom(               float amount = 1.0);
  void input_fire1(              float amount = 1.0);
  void input_fire2(              float amount = 1.0);
  void input_toggle_fullscreen(  float amount = 1.0);
  void input_toggle_help(        float amount = 1.0);
  void input_toggle_hud(         float amount = 1.0);
  void input_graphics_nicest(    float amount = 1.0);
  void input_graphics_compromise(float amount = 1.0);
  void input_graphics_fastest(   float amount = 1.0);
  #ifndef NDEBUG
    void input_cheat_god(        float amount = 1.0);
    void input_cheat_invisible(  float amount = 1.0);
    void input_cheat_noclip(     float amount = 1.0);
  #endif
};

#endif // GAMEPLAYER_H_INCLUDED
