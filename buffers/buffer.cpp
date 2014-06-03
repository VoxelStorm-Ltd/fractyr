#include "buffer.h"
#include <iostream>

buffer::buffer() {
  /// Default constructor
}

buffer::~buffer() {
  /// Default destructor
  destroy();
}

void buffer::init() {
  /// Allocate new buffers after context switch
  if(vbo == 0 &&
     ibo == 0) {
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
  }
  // do not mark initialised = true here, leave that up to the final overriding classes after they finish setup fully
}

void buffer::destroy() {
  /// Unallocate all existing buffers in preparation for context switch
  /// Note: this should be safe to call even on null buffers
  if(vbo != 0 ||
     ibo != 0) {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    vbo = 0;
    ibo = 0;
  }
  initialised = false;
}

void buffer::load_shader() {
  /// Load and initialise the shaders for this object
  std::cout << "ERROR: virtual base function " << __PRETTY_FUNCTION__ << " called; this should never happen" << std::endl;
}
void buffer::destroy_shader() {
  /// Clean up the shaders for this object
  std::cout << "ERROR: virtual base function " << __PRETTY_FUNCTION__ << " called; this should never happen" << std::endl;
}
void buffer::render() const {
  /// Set up the buffers and upload appropriate data
  std::cout << "ERROR: virtual base function " << __PRETTY_FUNCTION__ << " called; this should never happen" << std::endl;
}
