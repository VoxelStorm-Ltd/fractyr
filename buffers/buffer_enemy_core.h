#ifndef BUFFER_ENEMY_CORE_H_INCLUDED
#define BUFFER_ENEMY_CORE_H_INCLUDED

#include "buffer.h"
#include <vector>
#include "vmath.h"

class buffer_enemy_core : public buffer {
public:
  static GLuint shader;                                // shader program for this buffer type

  struct vertex {
    Vector3<GLfloat> coords;
    Vector3<GLfloat> normal;
    Vector4<GLfloat> colour;
    vertex(Vector3<GLfloat> coords,
           Vector3<GLfloat> normal,
           Vector4<GLfloat> colour)
    : coords(coords),
      normal(normal),
      colour(colour) {
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
  void cuboid(Vector3f const &pos, Vector3f const &size, Vector4f const &colour, std::vector<vertex> &vbodata, std::vector<GLuint> &ibodata) const;
};

#endif // BUFFER_ENEMY_CORE_H_INCLUDED
