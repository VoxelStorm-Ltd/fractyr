#ifndef BUFFER_SHARD_H_INCLUDED
#define BUFFER_SHARD_H_INCLUDED

#include "buffer.h"
#include <vector>
#include "vectorstorm/vectorstorm.h"

class buffer_shard : public buffer {
public:
  static GLuint shader;                                                         // shader program for this buffer type

  struct vertex {
    vector3<GLfloat> coords;
    vector3<GLfloat> normal;
    vector4<GLfloat> colour;
    vertex(vector3<GLfloat> const &this_coords,
           vector3<GLfloat> const &this_normal,
           vector4<GLfloat> const &this_colour)
    : coords(this_coords),
      normal(this_normal),
      colour(this_colour) {
      /// specific constructor
    }
  };
private:
  // per-vertex attribute indices
  static GLuint attrib_coords;
  static GLuint attrib_normal;
  static GLuint attrib_colour;
public:
  buffer_shard();
  ~buffer_shard() override final;
  static void load_shader();
  static void destroy_shader();
  void setup();
  void render() const override final;
};

#endif // BUFFER_SHARD_H_INCLUDED
