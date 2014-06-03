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
    vertex(Vector3<GLfloat> coords,
           Vector3<GLfloat> normal)
    : coords(coords),
      normal(normal) {
      /// specific constructor
    }
  };
private:
  // per-vertex attribute indices
  static GLuint attrib_coords;
  static GLuint attrib_normal;
public:
  buffer_chunk();
  ~buffer_chunk()       override final;
  void load_shader()    override final;
  void destroy_shader() override final;
  void setup(std::vector<vertex> const &vbodata, std::vector<GLuint> const &ibodata);
  void render() const   override final;
};

#endif // BUFFER_CHUNK_H_INCLUDED
