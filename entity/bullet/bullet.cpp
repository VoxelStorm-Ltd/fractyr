#include "bullet.h"

bullet::bullet(world &parent_world,
               chunk *parent_chunk,
               Vector3f const &position,
               Quatf const &orientation)
  : entity(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
}

bullet::~bullet() {
  /// Default destructor
}
