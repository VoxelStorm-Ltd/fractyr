#include "plasma.h"
#include "buffer_plasma.h"

buffer_plasma plasma::buf;

plasma::plasma(world &parent_world,
               chunk *parent_chunk,
               Vector3f const &position,
               Quatf const &orientation,
               Vector3f const &ship_velocity)
  : bullet(parent_world, parent_chunk, position, orientation) {
  /// Default constructor

  velocity = Vector3f(0.0, 0.0, 1.0);
  velocity.rotate(orientation);
  velocity += ship_velocity;
}

plasma::~plasma() {
  /// Default destructor
}

void plasma::render() const {
  /// Draw this in the right place
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  buf.render();
  glPopMatrix();
}
