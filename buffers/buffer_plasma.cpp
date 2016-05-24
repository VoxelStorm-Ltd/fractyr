#include "buffer_plasma.h"
#include <iostream>
//#include "blob_loader.h"
#include "shader_load.h"

// linked binary resource blob symbols using blob_loader.h
BLOB_LOAD(shaders_plasma_vert_glsl);
BLOB_LOAD(shaders_plasma_frag_glsl);

GLuint buffer_plasma::shader = 0;
GLuint buffer_plasma::attrib_coords = 0;
GLuint buffer_plasma::attrib_colour = 0;

buffer_plasma::buffer_plasma() {
  /// Default constructor
}

buffer_plasma::~buffer_plasma() {
  /// Default destructor
  destroy_shader();
}

void buffer_plasma::load_shader() {
  /// Load and initialise the terrain shaders
  if(shader != 0) {
    return;                                                                     // only load the shader once
  }
  shader = shader_load(std::string(reinterpret_cast<char const*>(BLOB(shaders_plasma_vert_glsl)),
                                   BLOB_SIZE(                         shaders_plasma_vert_glsl)),
                       std::string(reinterpret_cast<char const*>(BLOB(shaders_plasma_frag_glsl)),
                                   BLOB_SIZE(                         shaders_plasma_frag_glsl)));
  if(shader == GL_FALSE) {
    std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": Failed to load shaders, exiting." << std::endl;
    exit(EXIT_FAILURE);
  }
  // cache attribute and uniform indices
  attrib_coords = glGetAttribLocation(shader, "coords");
  attrib_colour = glGetAttribLocation(shader, "colour");
}

void buffer_plasma::destroy_shader() {
  /// Clean up the terrain shaders
  if(shader != 0) {
    glDeleteProgram(shader);
    shader = 0;
  }
}

void buffer_plasma::setup() {
  /// Set up the buffers and upload appropriate data
  #ifndef NDEBUG
    if(vbo == 0 || ibo == 0) {
      std::cout << "WARNING: " << __PRETTY_FUNCTION__ << ": buffer not previously initialised, initialising now - fix this for release!" << std::endl;
      init();
    }
    if(shader == 0) {
      std::cout << "WARNING: " << __PRETTY_FUNCTION__ << ": shader not previously loaded, loading now - fix this for release!" << std::endl;
      load_shader();
    }
  #endif
  std::vector<vertex> vbodata;
  std::vector<GLuint> ibodata;

  vbodata.reserve(4);
  ibodata.reserve(12);

  unsigned int offset = vbodata.size();
  vbodata.emplace_back(Vector3f( 0.2, 0.0,  0.0), Vector4f(1.0, 1.0, 0.0, 0.8));
  vbodata.emplace_back(Vector3f( 0.0, 0.0, -0.5), Vector4f(1.0, 1.0, 0.0, 1.0));
  vbodata.emplace_back(Vector3f(-0.2, 0.0,  0.0), Vector4f(1.0, 1.0, 0.0, 0.8));
  vbodata.emplace_back(Vector3f( 0.0, 0.0,  4.0), Vector4f(1.0, 1.0, 0.0, 0.2));
  ibodata.emplace_back(offset + 0);                                             // front face
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 0);                                             // back face
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 0);

  vbodata.shrink_to_fit();
  ibodata.shrink_to_fit();

  #ifndef NDEBUG
    std::cout << "Uploading " << vbodata.size() << " verts, " << ibodata.size() << " indices to plasma vbo ("
              << (vbodata.size() * sizeof(vertex)) / 1024 << "KB, "
              << (ibodata.size() * sizeof(vertex)) / 1024 << "KB)" << std::endl;
  #endif

  numverts = ibodata.size();
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vbodata.size() * sizeof(vertex), &vbodata[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibodata.size() * sizeof(GLuint), &ibodata[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  initialised = true;
}

void buffer_plasma::render() const {
  /// Render the buffers for this object in the appropriate way
  glUseProgram(shader);
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glEnableVertexAttribArray(attrib_coords);
  glEnableVertexAttribArray(attrib_colour);
  glVertexAttribPointer(attrib_coords, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  glVertexAttribPointer(attrib_colour, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::colour)));

  glDrawElements(GL_TRIANGLES, numverts, GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glDisableVertexAttribArray(attrib_coords);
  glDisableVertexAttribArray(attrib_colour);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
