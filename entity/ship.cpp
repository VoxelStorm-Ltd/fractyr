#include "ship.h"

ship::ship(world &parent_world, chunk *parent_chunk, Vector3f const &position)
  : entity(parent_world, parent_chunk, position) {
  /// Default constructor
}

ship::~ship() {
  /// Default destructor
}

void ship::accelerate(Vector3f accel) {
  accel.rotate(orientation);
  velocity += accel * acceleration;
}
