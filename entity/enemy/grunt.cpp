#include "grunt.h"
#include "buffer_enemy_grunt.h"

buffer_enemy_grunt grunt::buf;

grunt::grunt(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation)
  : enemy(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
}

grunt::~grunt() {
  /// Default destructor
}

void grunt::update() {
  /// Update this enemy's AI actions
  // TODO

  enemy::update();
}
