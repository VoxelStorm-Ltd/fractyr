#ifndef BUFFER_CHUNK_H_INCLUDED
#define BUFFER_CHUNK_H_INCLUDED

#include "buffer.h"
#include <vector>
#include "vmath.h"

class buffer_chunk : public buffer {
public:
  static GLuint shader;                                // shader program for this buffer type

  struct vertex {
    Vector3<GLfloat> coords;
    Vector3<GLfloat> normal;
    Vector3<GLfloat> colour;
    vertex(Vector3<GLfloat> coords,
           Vector3<GLfloat> normal,
           Vector3<GLfloat> colour)
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
  buffer_chunk();
  ~buffer_chunk()       override final;
  static void load_shader();
  static void destroy_shader();
  void setup(std::vector<vertex> const &vbodata, std::vector<GLuint> const &ibodata);
  void render() const   override final;
};

#endif // BUFFER_CHUNK_H_INCLUDED
