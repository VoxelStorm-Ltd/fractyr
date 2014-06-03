#include "chunk.h"
#include <random>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "entity.h"
#include "universe.h"

chunk::chunk(Vector3i const &chunk_coords, world &parent)
  : coords(chunk_coords),
    parent(&parent) {
  /// Default constructor
  buf.init();
  buf.load_shader();
  setup();
}

chunk::~chunk() {
  /// Default destructor
  for(auto &e : entities) {
    delete e;
  }
}

Vector3f chunk::check_collision(Vector3f const &coords, float radius) const {
  /// Check if a given point is colliding, and if so, return a normal vector to the collision surface
  // NOTE: coords can be less than 0 or greater than chunk::size by up to radius

  // TODO (note: use branch prediction hints for collision checks)

  return Vector3f(0.0, 0.0, 0.0);
}

void chunk::update() {
  /// Update every entity in this chunk
  for(auto &e : entities) {
    e->update();
  }
}

void chunk::add_entity(entity *thisentity) {
  /// Add this entity to our list
  entities.push_back(thisentity);
}

void chunk::remove_entity(entity *thisentity) {
  /// Take this entity out of our list
  //entities.erase(std::remove(entities.begin(), entities.end(), thisentity), entities.end());
  // should be faster:
  entities.erase(std::find(entities.begin(), entities.end(), thisentity));
  // TODO: for larger entity lists, instead sort and use std::binary_search
}

void chunk::render(Vector3i const &view_chunk_coords) const {
  /// Draw the contents of this chunk as viewed from coords
  Vector3i offset(coords - view_chunk_coords);
  glPushMatrix();
  //glTranslatef(-offset.x * size, -offset.y * size, -offset.z * size);

  buf.render();

  for(auto &e : entities) {
    e->render();
  }

  glPopMatrix();
}

void chunk::refresh() {
  /// Refresh the buffers for this chunk
  buf.destroy();
  buf.destroy_shader();
  buf.init();
  buf.load_shader();
  setup();
}

void chunk::setup() {
  std::vector<buffer_chunk::vertex> vbodata;
  std::vector<GLuint>               ibodata;

  std::uniform_real_distribution<float> dist_chunkwide(size * 0.25, size * 0.75);
  std::uniform_real_distribution<float> dist_tri(-1.0, 1.0);

  // placeholder: spam random triangles
  for(unsigned int i = 0; i != 10000; ++i) {
    Vector3f const coord0(dist_chunkwide(universe::randomgen), dist_chunkwide(universe::randomgen), dist_chunkwide(universe::randomgen));
    Vector3f const coord1(coord0 + Vector3f(dist_tri(universe::randomgen), dist_tri(universe::randomgen), dist_tri(universe::randomgen)));
    Vector3f const coord2(coord0 + Vector3f(dist_tri(universe::randomgen), dist_tri(universe::randomgen), dist_tri(universe::randomgen)));

    Vector3f const normal = (coord1 - coord0).crossProduct(coord2 - coord0);

    // front face
    unsigned int offset = vbodata.size();
    vbodata.emplace_back(coord0, normal);
    vbodata.emplace_back(coord1, normal);
    vbodata.emplace_back(coord2, normal);
    ibodata.emplace_back(offset + 0);
    ibodata.emplace_back(offset + 1);
    ibodata.emplace_back(offset + 2);

    // back face
    offset = vbodata.size();
    vbodata.emplace_back(coord0, -normal);
    vbodata.emplace_back(coord2, -normal);
    vbodata.emplace_back(coord1, -normal);
    ibodata.emplace_back(offset + 0);
    ibodata.emplace_back(offset + 1);
    ibodata.emplace_back(offset + 2);
  }

  buf.setup(vbodata, ibodata);
}
