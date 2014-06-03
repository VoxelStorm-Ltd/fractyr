#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class buffer {
  /// Polymorphic base class for graphics buffer objects, their shaders and render functions
protected:
  GLuint vbo      = 0;                                // vertex buffer object
  GLuint ibo      = 0;                                // index buffer object
  GLuint numverts = 0;                                // number of vertices in the VBO
public:
  bool initialised = false;                           // whether this buffer has been properly initialised
protected:
  buffer();
public:
  virtual ~buffer();

  virtual void init();
  virtual void destroy();
  virtual void render() const;
};

#endif // BUFFER_H_INCLUDED
