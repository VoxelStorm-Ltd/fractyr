#include "bullet.h"

bullet::bullet(world &this_parent_world,
               chunk *parent_chunk,
               vector3f const &this_position,
               quatf const &this_orientation)
  : entity(this_parent_world, parent_chunk, this_position, this_orientation) {
  /// Default constructor
}

bullet::~bullet() {
  /// Default destructor
}

void bullet::collided_with(entity *other __attribute__((unused))) {
  energy = 0;                                                                   // Destroy this bullet.
}

entity::entity_type bullet::get_entity_type() const {
  return entity::entity_type::BULLET;
}
