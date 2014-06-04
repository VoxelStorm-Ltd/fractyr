#include "grunt.h"
#include "buffer_enemy_grunt.h"

buffer_enemy_grunt grunt::buf;

grunt::grunt(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation)
  : enemy(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  radius = 3.0;
  mass = 1000.0;
}

grunt::~grunt() {
  /// Default destructor
}

void grunt::render() const {
  /// Render this entity
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glMultMatrixf(orientation_conjugate.transform());
  buf.render();
  glPopMatrix();
}

void grunt::update() {
  /// Update this enemy's AI actions
  // TODO
  velocity.x += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
  velocity.y += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
  velocity.z += (rand()  /static_cast<float>(RAND_MAX) - 0.5) / 100.0;

  enemy::update();
}
