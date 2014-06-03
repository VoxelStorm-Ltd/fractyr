#include "chunk.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "entity.h"

chunk::chunk(Vector3i const &chunk_coords)
  : coords(chunk_coords) {
  /// Default constructor
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

void chunk::render(Vector3i const &view_chunk_coords) const {
  /// Draw the contents of this chunk as viewed from coords
  Vector3i offset(coords - view_chunk_coords);
  glPushMatrix();
  glTranslatef(offset.x * size, offset.y * size, offset.z * size);

  // TODO

  for(auto &e : entities) {
    e->render();
  }

  glPopMatrix();
}
