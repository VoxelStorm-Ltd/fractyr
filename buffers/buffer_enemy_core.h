#ifndef BUFFER_ENEMY_CORE_H_INCLUDED
#define BUFFER_ENEMY_CORE_H_INCLUDED

#include "buffer.h"
#include <vector>
#include "vectorstorm/vectorstorm.h"

class buffer_enemy_core : public buffer {
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
  buffer_enemy_core();
  ~buffer_enemy_core() override final;
  static void load_shader();
  static void destroy_shader();
  void setup();
  void render() const override final;
  void cuboid(vector3f const &pos, vector3f const &size, vector4f const &colour, std::vector<vertex> &vbodata, std::vector<GLuint> &ibodata) const;
};

#endif // BUFFER_ENEMY_CORE_H_INCLUDED
