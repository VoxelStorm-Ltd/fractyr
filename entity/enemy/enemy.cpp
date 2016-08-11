#include "enemy.h"
//#include "world.h"
//#include "chunk.h"

enemy::enemy(world &this_parent_world,
             chunk *parent_chunk,
             vector3f const &this_position,
             quatf const &this_orientation)
  : ship(this_parent_world, parent_chunk, this_position, this_orientation) {
  /// Default constructor
}

enemy::~enemy() {
  /// Default destructor
}

void enemy::update() {
  /// Update this enemy's AI actions
  // TODO

  ship::update();
}

entity::entity_type enemy::get_entity_type() const {
  return entity::entity_type::ENEMY;
}
