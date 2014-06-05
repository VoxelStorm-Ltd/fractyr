#include "gameplayer.h"
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void gameplayer::pollcontrols(GLFWwindow *thiswindow) {
  /// Any controls where we just care about the current state, not keyup/down events
  if(gamepad != -1) {
    // only poll gamepad / joystick components if we have one activated
    axes =    glfwGetJoystickAxes(   gamepad, &axiscount);     // poll the axes
    buttons = glfwGetJoystickButtons(gamepad, &buttoncount);   // poll the buttons
  }
  for(auto &binding : bindings) {
    switch(binding.type) {
    case controlbinding::controltype::KEYBOARD:
      if(glfwGetKey(thiswindow, binding.control) == GLFW_PRESS) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    case controlbinding::controltype::MOUSE_AXIS:
      // TODO
      break;
    case controlbinding::controltype::MOUSE_BUTTON:
      if(glfwGetMouseButton(thiswindow, binding.control) == GLFW_PRESS) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    case controlbinding::controltype::MOUSE_SCROLL:
      // see callback_scroll
      break;
    case controlbinding::controltype::JOYSTICK_AXIS_FULL:
      if(fabsf(axes[binding.control]) > binding.deadzone) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(axes[binding.control] * binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    case controlbinding::controltype::JOYSTICK_AXIS_HALF_POS:
      if(axes[binding.control] > binding.deadzone) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(axes[binding.control] * binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    case controlbinding::controltype::JOYSTICK_AXIS_HALF_NEG:
      if(-axes[binding.control] > binding.deadzone) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(-axes[binding.control] * binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    case controlbinding::controltype::JOYSTICK_BUTTON:
      if(buttons[binding.control]) {
        if(binding.helddown && !binding.repeat) {
          break;                            // don't repeat
        }
        (this->*binding.function)(binding.scale);
        binding.helddown = true;
      } else {
        binding.helddown = false;
      }
      break;
    }
  }
}
