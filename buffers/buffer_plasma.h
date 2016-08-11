#ifndef BUFFER_PLASMA_H_INCLUDED
#define BUFFER_PLASMA_H_INCLUDED

#include "buffer.h"
#include <vector>
#include "vectorstorm/vectorstorm.h"

class buffer_plasma : public buffer {
public:
  static GLuint shader;                                                         // shader program for this buffer type

  struct vertex {
    vector3<GLfloat> coords;
    vector4<GLfloat> colour;
    vertex(vector3<GLfloat> const &this_coords,
           vector4<GLfloat> const &this_colour)
    : coords(this_coords),
      colour(this_colour) {
      /// specific constructor
    }
  };
private:
  // per-vertex attribute indices
  static GLuint attrib_coords;
  static GLuint attrib_colour;
public:
  buffer_plasma();
  ~buffer_plasma() override final;
  static void load_shader();
  static void destroy_shader();
  void setup();
  void render() const override final;
};

#endif // BUFFER_PLASMA_H_INCLUDED
