#include "gameplayer.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>
#include "vmath.h"
#include "soundstorm.h"
#include "oculusstorm.h"
#include "universe.h"
#include "ship.h"

extern GLFWwindow *window_main;
extern oculusstorm *oculus;
extern soundstorm sound;
extern universe root;
//extern FTFont *font_label;

gameplayer::gameplayer()
  : mouse_sensitivity(0.005, 0.005),
    windowsize(800, 600) {
  /// Default constructor
  std::cout << "Initialising player..." << std::endl;
  update_fov(fov_limit_max);

  helptext.emplace_back("CONTROLS");
  helptext.emplace_back("");
  helptext.emplace_back("TODO");

}

gameplayer::~gameplayer() {
  /// Default destructor
}

void gameplayer::cache_matrix(double nearplane, double farplane) {
  /// Pre-compute the projection matrix and store it
  /// This needs to be executed within a valid OpenGL context
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  double top    = tan(fov_angle_rad * 0.5) * nearplane;
  double bottom = -top;
  double left   = bottom * aspect_ratio;
  double right  = top    * aspect_ratio;
  glFrustum(left, right, bottom, top, nearplane, farplane);
  float temp_matrix[16];
  glGetFloatv(GL_PROJECTION_MATRIX, temp_matrix);
  projection = Matrix4f::fromColumnMajorArray(temp_matrix);
}

void gameplayer::setup_render_perspective() {
  /// Set up the projection matrix to the correct specifications
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(projection);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // dimension source: http://cdn.kk.org.s3.amazonaws.com/cooltools/2013/01/human-dimensions-6.jpeg
  // tweaked based on own measurements & tests:
  glTranslatef(0.0, -0.25, 0.13);                                   // neck model
  glRotatef(rotation_pitch, 1.0, 0.0, 0.0);                         // head rotation
  glRotatef(rotation_yaw + pre_rotation, 0.0, 1.0, 0.0);
  //sound.set_listener_rotation(Quatf::fromAxisRot(Vector3f(0.0, -1.0, 0.0), rotation_yaw + pre_rotation));   // we don't care about pitch here
}

void gameplayer::setup_render_oculus_left() {
  /// Oculus Rift perspective rendering, left eye
  oculus->setup_left();
  glTranslatef(0.0, -0.25, 0.13);                                   // neck model (see above)
  glMultMatrixf(oculus->getmatrix().inverse());
  glRotated(pre_rotation, 0.0, 1.0, 0.0);
  //sound.set_listener_rotation(oculus->getquat());                   // only do this in one eye's setup to avoid duplication
}

void gameplayer::setup_render_oculus_right() {
  /// Oculus Rift perspective rendering, right eye
  oculus->setup_right();
  glTranslatef(0.0, -0.25, 0.13);                                   // neck model (see above)
  glMultMatrixf(oculus->getmatrix().inverse());
  glRotated(pre_rotation, 0.0, 1.0, 0.0);
}

void gameplayer::setup_render_oculus_left_locked() {
  /// Oculus Rift perspective rendering, left eye, ignoring tracker data
  oculus->setup_left();
  glTranslatef(0.0, -0.25, 0.13);                                   // neck model (see above)
  glRotated(pre_rotation, 0.0, 1.0, 0.0);
}

void gameplayer::setup_render_oculus_right_locked() {
  /// Oculus Rift perspective rendering, right eye, ignoring tracker data
  oculus->setup_right();
  glTranslatef(0.0, -0.25, 0.13);                                   // neck model (see above)
  glRotated(pre_rotation, 0.0, 1.0, 0.0);
}

void gameplayer::setup_render_ortho() {
  /// set up 2D orthogonal rendering
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, windowsize.x, 0, windowsize.y, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
}

void gameplayer::setup_render_ortho_restore() {
  /// bring back to a sensible state after ortho rendering
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// no need to restore matrices for drawing if we're going to set them up again next frame, but they are needed for picking
  glEnable(GL_DEPTH_TEST);
}

void gameplayer::render_hud() {
  /// Render the heads-up display for the player
  if(__builtin_expect(show_help, 0)) {      // branch prediction hint: unlikely
    unsigned int constexpr insetouter = 30;
    unsigned int constexpr insetinner = 8;
    setup_render_ortho();
    glColor4f(0.0, 0.0, 0.0, 0.25);
    glBegin(GL_QUADS);
    glVertex2i(               insetouter,                insetouter);
    glVertex2i(windowsize.x - insetouter,                insetouter);
    glVertex2i(windowsize.x - insetouter, windowsize.y - insetouter);
    glVertex2i(               insetouter, windowsize.y - insetouter);
    glEnd();
    int yoffset = 15;
    glColor4f(1.0, 1.0, 1.0, 1.0);
    for(std::string line : helptext) {
      //font_label->Render(line.c_str(), line.length(), FTPoint(insetouter + insetinner, windowsize.y - insetouter - insetinner - yoffset), FTPoint(), FTGL::RENDER_FRONT);
      yoffset += 15;
    }
    glColor4f(0.0, 1.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(               insetouter,                insetouter);
    glVertex2i(windowsize.x - insetouter,                insetouter);
    glVertex2i(windowsize.x - insetouter, windowsize.y - insetouter);
    glVertex2i(               insetouter, windowsize.y - insetouter);
    glEnd();
    setup_render_ortho_restore();
    return;
  }
  if(__builtin_expect(!show_hud, 0)) {      // branch prediction hint: unlikely
    return;   // if HUD is hidden, skip this
  }

  // draw some crosshairs at the cursor location
  int constexpr cursorsize = 8;
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  glVertex2i(cursorpos.x, cursorpos.y - cursorsize);    // crosshair
  glVertex2i(cursorpos.x, cursorpos.y + cursorsize);
  glVertex2i(cursorpos.x - cursorsize, cursorpos.y);
  glVertex2i(cursorpos.x + cursorsize, cursorpos.y);
  // TODO: VBO this up
  glEnd();

  setup_render_ortho_restore();
}

void gameplayer::update_fov(double new_fov) {
  /// Update our field of view
  fov_angle = new_fov;
  fov_angle_rad = DEG2RAD(fov_angle);
  update_fov_ratio();
}

void gameplayer::update_fov_ratio() {
  /// Helper function to calculate field of view ratio from a field of view angle
  fov_ratio = tan(fov_angle_rad);
  //std::cout << "New FOV ratio: " << fov_ratio << std::endl;
}

Vector2i const &gameplayer::get_windowsize() const {
  return windowsize;
}

void gameplayer::update_window(Vector2i const &newwindowsize) {
  /// Resize the window to the new size, and update aspect ratio
  windowsize = newwindowsize;
  glViewport(0, 0, windowsize.x, windowsize.y);
  update_aspect_ratio();
}

void gameplayer::update_aspect_ratio() {
  /// Re-calculate our aspect ratio based on window size
  if(windowsize.x == 0 || windowsize.y == 0) {
    return;
  }
  aspect_ratio = static_cast<double>(windowsize.x) / static_cast<double>(windowsize.y);
  std::cout << "New window size " << windowsize << " aspect ratio " << aspect_ratio << std::endl;
  cache_matrix();   // update the cached projection matrix
}

void gameplayer::clamp_pitch() {
  /// Clamp view pitch angle
  double constexpr pitchmax = 90.0;
  if(rotation_pitch > pitchmax) {
    rotation_pitch = pitchmax;
  } else if(rotation_pitch < -pitchmax) {
    rotation_pitch = -pitchmax;
  }
}

float const &gameplayer::get_rotation_yaw() const {
  return rotation_yaw;
}
float const &gameplayer::get_rotation_pitch() const {
  return rotation_pitch;
}

void gameplayer::setup_input() {
  /// Scan for input devices
  std::cout << "Looking for input devices..." << std::endl;
  for(int checkpad = 0; checkpad != 16; ++checkpad) {
    if(glfwJoystickPresent(checkpad)) {
      std::cout << "  " << checkpad << ": " << glfwGetJoystickName(checkpad) << std::endl;
      gamepads.emplace_back(checkpad);
    }
  }
  if(gamepads.size() != 0) {
    select_gamepad(gamepads.front());
  }

  // default bindings
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_W,            &gameplayer::input_move_forward);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_UP,           &gameplayer::input_move_forward);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_S,            &gameplayer::input_move_back);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_DOWN,         &gameplayer::input_move_back);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_A,            &gameplayer::input_move_left);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_LEFT,         &gameplayer::input_move_left);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_D,            &gameplayer::input_move_right);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_RIGHT,        &gameplayer::input_move_right);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_Q,            &gameplayer::input_turn_left);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_E,            &gameplayer::input_turn_right);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_SPACE,        &gameplayer::input_move_up);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_LEFT_CONTROL, &gameplayer::input_move_down);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F11,          &gameplayer::input_toggle_fullscreen,   false);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F1,           &gameplayer::input_toggle_help,         false);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F2,           &gameplayer::input_toggle_hud,          false);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F5,           &gameplayer::input_graphics_nicest,     false);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F6,           &gameplayer::input_graphics_compromise, false);
  bind(controlbinding::controltype::KEYBOARD, GLFW_KEY_F7,           &gameplayer::input_graphics_fastest,    false);
  bind(controlbinding::controltype::MOUSE_SCROLL, 0,                 &gameplayer::input_zoom,                true, 0.0, -1.0);

  if(gamepad != -1) {
    // gamepad / joystick bindings only if we actually have one connected, so we don't need runtime checks
    // TODO
  }
}

void gameplayer::select_gamepad(int newgamepad) {
  /// Switch to a different gamepad, and update buttons and axes
  if(!glfwJoystickPresent(newgamepad)) {
    std::cout << "ERROR: input device number " << newgamepad << " not present." << std::endl;
    return;
  }
  gamepad = newgamepad;
  int axiscount = 0;
  int buttoncount = 0;
  axes = glfwGetJoystickAxes(gamepad, &axiscount);
  buttons = glfwGetJoystickButtons(gamepad, &buttoncount);
  std::cout << "Selected input device " << gamepad << ": " << glfwGetJoystickName(gamepad) << std::endl;
  std::cout << "  " << axiscount << " axes, " << buttoncount << " buttons" << std::endl;
}

void gameplayer::move_mouse(Vector2f const &mouse_pos) {
  /// React to mouse position updates
  // Quake mouselook code is here for reference: https://github.com/id-Software/Quake/blob/bf4ac424ce754894ac8f1dae6a3981954bc9852d/WinQuake/in_win.c
  Vector2f mouse_diff(mouse_pos - mouse_last);
  mouse_diff *= mouse_sensitivity * fov_angle;      // apply sensitivity
  //rotation_yaw = fmodf(rotation_yaw + mouse_diff.x, 360.0);
  //rotation_pitch += mouse_diff.y;
  //clamp_pitch();
  rotation_yaw   = 0.0;
  rotation_pitch = 0.0;

  if(__builtin_expect(!current_ship, 0)) {        // branch prediction: unlikely to not have a ship
    return;
  }
  current_ship->rotate(mouse_diff.x, mouse_diff.y);

  mouse_last = mouse_pos;
}

void gameplayer::bind(controlbinding::controltype type,
                      int control,
                      controlbinding::inputfunctiontype function,
                      bool repeat,
                      float deadzone,
                      float scale) {
  /// Create a new binding for the specified control
  bindings.push_back({type, control, function, deadzone, scale, repeat, false});
}

void gameplayer::unbind(controlbinding::controltype type, int control) {
  /// Remove any bindings to a specified control
  for(auto it = bindings.begin(); it != bindings.end();) {
    if(it->type == type && it->control == control) {
      it = bindings.erase(it);
    } else {
      ++it;
    }
  }
}

void gameplayer::unbind(controlbinding::inputfunctiontype function) {
  /// Remove any bindings to a specified function
  for(auto it = bindings.begin(); it != bindings.end();) {
    if(it->function == function) {
      it = bindings.erase(it);
    } else {
      ++it;
    }
  }
}

void gameplayer::update() {
  /// Poll controls and update input actions
  pollcontrols(window_main);
  // TODO: update player ship velocities
}

void gameplayer::input_move_forward(float amount) {
  current_ship->accelerate(Vector3f(0.0, 0.0, -amount));
}
void gameplayer::input_move_back(float amount) {
  current_ship->accelerate(Vector3f(0.0, 0.0, amount));
}
void gameplayer::input_move_left(float amount) {
  current_ship->accelerate(Vector3f(-amount, 0.0, 0.0));
}
void gameplayer::input_move_right(float amount) {
  current_ship->accelerate(Vector3f(amount, 0.0, 0.0));
}
void gameplayer::input_move_up(float amount) {
  current_ship->accelerate(Vector3f(0.0, amount, 0.0));
}
void gameplayer::input_move_down(float amount) {
  current_ship->accelerate(Vector3f(0.0, -amount, 0.0));
}
void gameplayer::input_turn_left(float amount) {
  pre_rotation = fmodf(pre_rotation - (2.0 * amount), 360.0);
}
void gameplayer::input_turn_right(float amount) {
  pre_rotation = fmodf(pre_rotation + (2.0 * amount), 360.0);
}
void gameplayer::input_zoom_in(float amount) {
  /// Narrow the view angle, amount must be positive
  //fov_angle -= 10.0 * amount;#
  fov_angle /= 1.0 + amount;
  if(fov_angle < fov_limit_min) {
    fov_angle = fov_limit_min;
  }
  update_fov(fov_angle);
  cache_matrix();   // update the cached projection matrix
}
void gameplayer::input_zoom_out(float amount) {
  /// Widen the view angle, amount must be positive
  //fov_angle += 10.0 * amount;
  fov_angle *= 1.0 + amount;
  if(fov_angle > fov_limit_max) {
    fov_angle = fov_limit_max;
  }
  update_fov(fov_angle);
  cache_matrix();   // update the cached projection matrix
}
void gameplayer::input_zoom(float amount) {
  /// As above but suitable for joystick and mousewheel use, can be given a negative amount
  //fov_angle += 10.0 * amount;
  if(amount > 0.0) {
    fov_angle *= 1.0 + amount;
  } else {
    fov_angle /= 1.0 - amount;
  }
  if(fov_angle < fov_limit_min) {
    fov_angle = fov_limit_min;
  }
  if(fov_angle > fov_limit_max) {
    fov_angle = fov_limit_max;
  }
  //std::cout << "DEBUG: FOV now " << fov_angle << std::endl;
  update_fov(fov_angle);
  cache_matrix();   // update the cached projection matrix
}
void gameplayer::input_toggle_fullscreen(float amount __attribute__((__unused__))) {
  root.toggle_fullscreen();
}
void gameplayer::input_toggle_help(float amount __attribute__((__unused__))) {
  show_help = !show_help;
}
void gameplayer::input_toggle_hud(float amount __attribute__((__unused__))) {
  show_hud = !show_hud;
}
void gameplayer::input_graphics_nicest(float amount __attribute__((__unused__))) {
  root.set_graphicslevel(universe::graphicsleveltype::NICEST);
}
void gameplayer::input_graphics_compromise(float amount __attribute__((__unused__))) {
  root.set_graphicslevel(universe::graphicsleveltype::COMPROMISE);
}
void gameplayer::input_graphics_fastest(float amount __attribute__((__unused__))) {
  root.set_graphicslevel(universe::graphicsleveltype::FASTEST);
}
