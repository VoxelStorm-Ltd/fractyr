#include "core.h"
#include "buffers/buffer_enemy_core.h"
#include "gameplayer.h"
#include "entity/particle/particle.h"

extern gameplayer player;

buffer_enemy_core core::buf;

core::core(world &parent_world,
             chunk *parent_chunk,
             vector3f const &position,
             quatf const &orientation,
             float scale)
  : enemy(parent_world, parent_chunk, position, orientation),
    scale(scale) {
  /// Default constructor
  radius = 5.0 * scale;
  mass = 1000.0 * scale;
  energy = 1000 * scale * scale;
}

core::~core() {
  /// Default destructor
}

void core::render() const {
  /// Render this entity
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glMultMatrixf(orientation_conjugate.transform());
  glScalef(scale, scale, scale);
  buf.render();
  glPopMatrix();
}

void core::update() {
  /// Update this enemy's AI actions
  // TODO: The tracking gets confused if (I think) the player crosses the z axis, slerp should deal with this fine but isn't for some reason.

  orientation *= quatf::from_axis_rot(vector3f(0.0, 0.0, 1.0), 1.0);            // spin fast
  orientation_conjugate = orientation.conjugate_copy();
  enemy::update();
}

void core::destroy() {
  /// Make a big explosion
  for(unsigned int p = 0; p != 100; ++p) {
    new particle(parent_world,
                 parent,
                 position,
                 orientation,
                 velocity + vector3f((static_cast<float>(rand()) / RAND_MAX) - 0.5,
                                     (static_cast<float>(rand()) / RAND_MAX) - 0.5,
                                     (static_cast<float>(rand()) / RAND_MAX) - 0.5) * 0.5);
  }
}

float core::get_collision_damage() const {
  return 100.0;
}

entity::entity_type core::get_entity_type() const {
  return entity::entity_type::CORE;
}
