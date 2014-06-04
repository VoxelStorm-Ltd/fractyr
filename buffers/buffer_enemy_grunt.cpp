#include "buffer_enemy_grunt.h"
#include "blob_loader.h"

// loader
GLuint shader_load(std::string const &shader_vertex_source, std::string const &shader_fragment_source);

// linked binary resource blob symbols using blob_loader.h
BLOB_LOAD(shaders_enemy_grunt_vert_glsl);
BLOB_LOAD(shaders_enemy_grunt_frag_glsl);

GLuint buffer_enemy_grunt::shader = 0;
GLuint buffer_enemy_grunt::attrib_coords = 0;
GLuint buffer_enemy_grunt::attrib_normal = 0;
GLuint buffer_enemy_grunt::attrib_colour = 0;

buffer_enemy_grunt::buffer_enemy_grunt() {
  /// Default constructor
}

buffer_enemy_grunt::~buffer_enemy_grunt() {
  /// Default destructor
  destroy_shader();
}

void buffer_enemy_grunt::load_shader() {
  /// Load and initialise the terrain shaders
  if(shader != 0) {
    return;         // only load the shader once
  }
  shader = shader_load(std::string(reinterpret_cast<char const*>(BLOB(shaders_enemy_grunt_vert_glsl)),
                                   BLOB_SIZE(                         shaders_enemy_grunt_vert_glsl)),
                       std::string(reinterpret_cast<char const*>(BLOB(shaders_enemy_grunt_frag_glsl)),
                                   BLOB_SIZE(                         shaders_enemy_grunt_frag_glsl)));
  if(shader == GL_FALSE) {
    std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": Failed to load shaders, exiting." << std::endl;
    exit(EXIT_FAILURE);
  }
  // cache attribute and uniform indices
  attrib_coords = glGetAttribLocation(shader, "coords");
  attrib_normal = glGetAttribLocation(shader, "normal");
  attrib_colour = glGetAttribLocation(shader, "colour");
}

void buffer_enemy_grunt::destroy_shader() {
  /// Clean up the terrain shaders
  if(shader != 0) {
    glDeleteProgram(shader);
    shader = 0;
  }
}

void buffer_enemy_grunt::setup() {
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

  vbodata.reserve(4 * 6);
  ibodata.reserve(6 * 6);

  float constexpr size = 3.0;
  float constexpr size_half = size / 2.0;

  Vector3f const coord000(-size_half, -size_half, -size_half);
  Vector3f const coord100(coord000 + Vector3f(size, 0.0f, 0.0f));
  Vector3f const coord010(coord000 + Vector3f(0.0f, size, 0.0f));
  Vector3f const coord110(coord000 + Vector3f(size, size, 0.0f));

  Vector3f const coord001(-size_half, -size_half, size_half);
  Vector3f const coord101(coord001 + Vector3f(size, 0.0f, 0.0f));
  Vector3f const coord011(coord001 + Vector3f(0.0f, size, 0.0f));
  Vector3f const coord111(coord001 + Vector3f(size, size, 0.0f));

  Vector3f const normal0( 0.0, 0.0, -1.0);
  Vector3f const normal1(-1.0, 0.0,  0.0);
  Vector3f const normal2( 0.0, 1.0,  0.0);

  Vector4f const colour(0.8, 0.0, 0.0, 1.0);

  // front face
  unsigned int offset = vbodata.size();
  vbodata.emplace_back(coord000, normal0, colour);
  vbodata.emplace_back(coord010, normal0, colour);
  vbodata.emplace_back(coord110, normal0, colour);
  vbodata.emplace_back(coord100, normal0, colour);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 0);

  // back face
  offset = vbodata.size();
  vbodata.emplace_back(coord001, -normal0, colour);
  vbodata.emplace_back(coord011, -normal0, colour);
  vbodata.emplace_back(coord111, -normal0, colour);
  vbodata.emplace_back(coord101, -normal0, colour);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 0);

  // Side face 1
  offset = vbodata.size();
  vbodata.emplace_back(coord000, normal1, colour);
  vbodata.emplace_back(coord001, normal1, colour);
  vbodata.emplace_back(coord011, normal1, colour);
  vbodata.emplace_back(coord010, normal1, colour);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 0);

  // Side face 2
  offset = vbodata.size();
  vbodata.emplace_back(coord100, -normal1, colour);
  vbodata.emplace_back(coord101, -normal1, colour);
  vbodata.emplace_back(coord111, -normal1, colour);
  vbodata.emplace_back(coord110, -normal1, colour);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 0);

  // top face
  offset = vbodata.size();
  vbodata.emplace_back(coord010, normal2, colour);
  vbodata.emplace_back(coord011, normal2, colour);
  vbodata.emplace_back(coord111, normal2, colour);
  vbodata.emplace_back(coord110, normal2, colour);
  ibodata.emplace_back(offset + 0);
  ibodata.emplace_back(offset + 1);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 2);
  ibodata.emplace_back(offset + 3);
  ibodata.emplace_back(offset + 0);

  // bottom face
  offset = vbodata.size();
  vbodata.emplace_back(coord000, -normal2, colour);
  vbodata.emplace_back(coord001, -normal2, colour);
  vbodata.emplace_back(coord101, -normal2, colour);
  vbodata.emplace_back(coord100, -normal2, colour);
  ibodata.emplace_back(offset + 0);
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

void buffer_enemy_grunt::render() const {
  /// Render the buffers for this object in the appropriate way
  glUseProgram(shader);
  glBindBuffer(GL_ARRAY_BUFFER,         vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glEnableVertexAttribArray(attrib_coords);
  glEnableVertexAttribArray(attrib_normal);
  glEnableVertexAttribArray(attrib_colour);
  glVertexAttribPointer(attrib_coords, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::coords)));
  glVertexAttribPointer(attrib_normal, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::normal)));
  glVertexAttribPointer(attrib_colour, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<GLvoid*>(offsetof(vertex, vertex::colour)));

  glDrawElements(GL_TRIANGLES, numverts, GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glDisableVertexAttribArray(attrib_coords);
  glDisableVertexAttribArray(attrib_normal);
  glDisableVertexAttribArray(attrib_colour);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
