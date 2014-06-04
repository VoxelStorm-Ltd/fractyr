#include "enemy.h"

enemy::enemy(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation)
  : ship(parent_world, parent_chunk, position, orientation) {
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
