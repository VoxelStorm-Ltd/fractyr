#include "grunt.h"
#include "buffer_enemy_grunt.h"
#include "weapon.h"
#include "gameplayer.h"

extern gameplayer player;

buffer_enemy_grunt grunt::buf;

grunt::grunt(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation)
  : enemy(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  radius = 3.0;
  mass = 1000.0;
  energy = 400;
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

  // TODO: Take relative velocities and distance into account to correctly lead shots.
  // TODO: The tracking gets confused if (I think) the player crosses the z axis, slerp should deal with this fine but isn't for some reason.

  if((get_world_position() - player.current_ship->get_world_position()).lengthSq() < 40000) {
    velocity.x += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.y += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.z += (rand()  /static_cast<float>(RAND_MAX) - 0.5) / 100.0;

    Quatf target_orientation = Quatf::fromMatrix(Matrix4f::createLookAt(get_world_position(), player.current_ship->get_world_position(), Vector3f(0, 0, 1)));
    orientation = orientation.slerp(0.05, target_orientation);
    orientation.normalise();
    orientation_conjugate = orientation.conjugate_copy();

    for (auto weapon : weapons) {
      weapon->fire();
    }
  } else {
    velocity = Vector3f(0.0, 0.0, 0.0);
  }
  enemy::update();
}

float grunt::get_collision_damage() const {
  return 100.0;
}
